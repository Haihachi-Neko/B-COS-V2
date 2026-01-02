#ifndef BARCODE_H
#define BARCODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Prototyp der Funktion, um einen Code39-Barcode als SVG-Zeichenkette zu erzeugen.
// text: Der zu kodierende Text (Großbuchstaben, Zahlen und die Zeichen -. $/+%).
// scale: Die Breite eines schmalen Balkens in Pixel. Die breiten Balken sind 3x so breit.
// height: Die Höhe des Barcodes in Pixel.
// Gibt eine dynamisch zugewiesene Zeichenkette mit den SVG-Daten zurück.
// Der Aufrufer ist dafür verantwortlich, den Speicher mit free() freizugeben.
// Gibt NULL bei einem Fehler zurück (z.B. ungültiges Zeichen).
char* generate_code39_svg(const char* text, int scale, int height);

// Prototyp der Funktion, um einen NW-7-Barcode als SVG-Zeichenkette zu erzeugen.
// text: Der zu kodierende Text (Zahlen, -$:/.).
// start_char: Das Startzeichen (A, B, C oder D).
// stop_char: Das Stoppzeichen (A, B, C oder D).
// scale: Die Breite eines schmalen Balkens in Pixel. Die breiten Balken sind 3x so breit.
// height: Die Höhe des Barcodes in Pixel.
// Gibt eine dynamisch zugewiesene Zeichenkette mit den SVG-Daten zurück.
// Der Aufrufer ist dafür verantwortlich, den Speicher mit free() freizugeben.
// Gibt NULL bei einem Fehler zurück (z.B. ungültiges Zeichen).
char* generate_nw7_svg(const char* text, char start_char, char stop_char, int scale, int height);


#ifdef BARCODE_IMPLEMENTATION

// Interne Struktur zur Abbildung von Zeichen auf ihre Code39-Muster.
// Ein Muster ist eine 9-stellige Zeichenkette aus '1' (breit) und '0' (schmal),
// die 5 Balken und 4 Lücken darstellt.
static const struct {
    char chr;
    const char* pattern;
} CODE39_PATTERNS[] = {
    {'0', "000110100"}, {'1', "100100001"}, {'2', "001100001"},
    {'3', "101100000"}, {'4', "000110001"}, {'5', "100110000"},
    {'6', "001110000"}, {'7', "000100101"}, {'8', "100100100"},
    {'9', "001100100"}, {'A', "100001001"}, {'B', "001001001"},
    {'C', "101001000"}, {'D', "000011001"}, {'E', "100011000"},
    {'F', "001011000"}, {'G', "000001101"}, {'H', "100001100"},
    {'I', "001001100"}, {'J', "000011100"}, {'K', "100000011"},
    {'L', "001000011"}, {'M', "101000010"}, {'N', "000010011"},
    {'O', "100010010"}, {'P', "001010010"}, {'Q', "000000111"},
    {'R', "100000110"}, {'S', "001000110"}, {'T', "000010110"},
    {'U', "110000001"}, {'V', "011000001"}, {'W', "111000000"},
    {'X', "010010001"}, {'Y', "110010000"}, {'Z', "011010000"},
    {'-', "010000101"}, {'.', "110000100"}, {' ', "011000100"},
    {'$', "010101000"}, {'/', "010100010"}, {'+', "010001010"},
    {'%', "000101010"}, {'*', "010010100"} // Start/Stop
};

// ... (Code39-Funktionen bleiben unverändert)

// Interne Struktur zur Abbildung von Zeichen auf ihre NW-7-Muster.
// Jedes Muster besteht aus 4 Balken und 3 Lücken. '1' für breit, '0' für schmal.
static const struct {
    char chr;
    const char* pattern;
} NW7_PATTERNS[] = {
    {'0', "0000011"}, {'1', "0000110"}, {'2', "0001001"},
    {'3', "1100000"}, {'4', "0010010"}, {'5', "1000010"},
    {'6', "0100001"}, {'7', "0100100"}, {'8', "0110000"},
    {'9', "1001000"}, {'-', "0001100"}, {'$', "0011000"},
    {':', "1000101"}, {'/', "1010001"}, {'.', "1010100"},
    {'+', "0010101"},
    // Start/Stopp-Zeichen
    {'A', "0011010"}, {'B', "0101001"}, {'C', "0001011"},
    {'D', "0001110"}
};

// Hilfsfunktion zum Finden des Musters für ein NW-7-Zeichen.
static const char* get_nw7_pattern(char c) {
    char upper_c = toupper(c);
    for (size_t i = 0; i < sizeof(NW7_PATTERNS) / sizeof(NW7_PATTERNS[0]); ++i) {
        if (NW7_PATTERNS[i].chr == upper_c) {
            return NW7_PATTERNS[i].pattern;
        }
    }
    return NULL;
}

// Hilfsfunktion zum Zeichnen eines Musters für NW-7
static void draw_nw7_pattern(char** p, const char* pattern, int* x, int h, int scale) {
    const int WIDE = 3 * scale;
    const int NARROW = scale;
    for (int i = 0; i < 7; ++i) {
        int width = (pattern[i] == '1') ? WIDE : NARROW;
        if (i % 2 == 0) { // Balken (gerade Indizes: 0, 2, 4, 6)
            *p += sprintf(*p, "  <rect x=\"%d\" y=\"0\" width=\"%d\" height=\"%d\" style=\"fill:black\"/>\n", *x, width, h);
        }
        *x += width;
    }
}

char* generate_nw7_svg(const char* text, char start_char, char stop_char, int scale, int height) {
    size_t num_chars = strlen(text);
    const int WIDE = 3 * scale;
    const int NARROW = scale;

    // 1. Eingabe validieren
    const char* start_pattern = get_nw7_pattern(start_char);
    const char* stop_pattern = get_nw7_pattern(stop_char);

    if (start_pattern == NULL || stop_pattern == NULL) {
        fprintf(stderr, "Fehler: Ungültiges Start- oder Stoppzeichen für NW-7.\n");
        return NULL;
    }
    // Überprüfen, ob die Start-/Stoppzeichen A, B, C oder D sind
    char upper_start = toupper(start_char);
    char upper_stop = toupper(stop_char);
    if ((upper_start < 'A' || upper_start > 'D') || (upper_stop < 'A' || upper_stop > 'D')) {
        fprintf(stderr, "Fehler: Start-/Stoppzeichen für NW-7 müssen A, B, C oder D sein.\n");
        return NULL;
    }


    for (size_t i = 0; i < num_chars; ++i) {
        if (get_nw7_pattern(text[i]) == NULL) {
            fprintf(stderr, "Fehler: Ungültiges Zeichen im Eingabetext für NW-7: %c\n", text[i]);
            return NULL;
        }
    }

    // 2. Gesamtbreite berechnen
    int total_width = 0;
    // Breite für Start- und Stoppzeichen (jeweils 7 Elemente)
    for (int i = 0; i < 7; i++) total_width += (start_pattern[i] == '1' ? WIDE : NARROW);
    for (int i = 0; i < 7; i++) total_width += (stop_pattern[i] == '1' ? WIDE : NARROW);


    // Breite für Datenzeichen (jeweils 7 Elemente)
    for (size_t i = 0; i < num_chars; ++i) {
        const char* pattern = get_nw7_pattern(text[i]);
        for (int j = 0; j < 7; ++j) {
            total_width += (pattern[j] == '1' ? WIDE : NARROW);
        }
    }

    // Breite für die Lücken zwischen den Zeichen (num_chars + 1 schmale Lücken)
    total_width += (num_chars + 1) * NARROW;

    // 3. Speicher für die SVG-Zeichenkette zuweisen
    size_t buffer_size = 2048 + (num_chars + 2) * 7 * 128; // großzügige Schätzung
    char* svg = (char*)malloc(buffer_size);
    if (svg == NULL) {
        fprintf(stderr, "Fehler: Speicher konnte nicht zugewiesen werden.\n");
        return NULL;
    }

    // 4. SVG-Inhalt generieren
    char* p = svg;
    p += sprintf(p, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" version=\"1.1\">\n", total_width, height);
    p += sprintf(p, "  <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" style=\"fill:white\"/>\n", total_width, height);

    int current_x = 0;

    // Start-Zeichen zeichnen
    draw_nw7_pattern(&p, start_pattern, &current_x, height, scale);

    // Datenzeichen zeichnen
    for (size_t i = 0; i < num_chars; ++i) {
        // Lücke zwischen den Zeichen (eine schmale Lücke)
        current_x += NARROW;
        const char* pattern = get_nw7_pattern(text[i]);
        draw_nw7_pattern(&p, pattern, &current_x, height, scale);
    }

    // Lücke vor dem Stoppzeichen
    current_x += NARROW;

    // Stopp-Zeichen zeichnen
    draw_nw7_pattern(&p, stop_pattern, &current_x, height, scale);

    p += sprintf(p, "</svg>\n");

    // Optional: Speicher neu zuweisen, um genau zu passen.
    char* final_svg = (char*)realloc(svg, strlen(svg) + 1);
    if (final_svg == NULL) {
        fprintf(stderr, "Fehler: Neuzuweisung des Speichers fehlgeschlagen.\n");
        free(svg);
        return NULL;
    }

    return final_svg;
}

// Hilfsfunktion zum Finden des Musters für ein Zeichen.
static const char* get_code39_pattern(char c) {
    // Stellt sicher, dass das Zeichen großgeschrieben ist, da Code39 dies erfordert.
    char upper_c = toupper(c);
    for (size_t i = 0; i < sizeof(CODE39_PATTERNS) / sizeof(CODE39_PATTERNS[0]); ++i) {
        if (CODE39_PATTERNS[i].chr == upper_c) {
            return CODE39_PATTERNS[i].pattern;
        }
    }
    return NULL;
}

char* generate_code39_svg(const char* text, int scale, int height) {
    size_t num_chars = strlen(text);
    const int WIDE = 3 * scale;
    const int NARROW = scale;

    // 1. Eingabe validieren und Gesamtbreite berechnen
    int total_width = 0;
    const char* start_stop_pattern = get_code39_pattern('*');

    for (int i = 0; i < num_chars; ++i) {
        if (get_code39_pattern(text[i]) == NULL) {
            fprintf(stderr, "Fehler: Ungültiges Zeichen im Eingabetext für Code39: %c\n", text[i]);
            return NULL;
        }
    }

    // Breite für Start/Stop-Zeichen
    for(int i = 0; i < 9; ++i) total_width += (start_stop_pattern[i] == '1' ? WIDE : NARROW);
    total_width *= 2; // für Start und Stop

    // Breite für Datenzeichen
    for(size_t i = 0; i < num_chars; ++i) {
        const char* pattern = get_code39_pattern(text[i]);
        for(int j = 0; j < 9; ++j) {
            total_width += (pattern[j] == '1' ? WIDE : NARROW);
        }
    }
    // Breite für Lücken zwischen den Zeichen (num_chars + 1 schmale Lücken)
    total_width += (num_chars + 1) * NARROW;


    // 2. Speicher für die SVG-Zeichenkette zuweisen
    size_t buffer_size = 2048 + (num_chars + 2) * 9 * 128; // großzügige Schätzung
    char* svg = (char*)malloc(buffer_size);
    if (svg == NULL) {
        fprintf(stderr, "Fehler: Speicher konnte nicht zugewiesen werden.\n");
        return NULL;
    }

    // 3. SVG-Inhalt generieren
    char* p = svg;
    p += sprintf(p, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" version=\"1.1\">\n", total_width, height);
    p += sprintf(p, "  <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" style=\"fill:white\"/>\n", total_width, height);

    int current_x = 0;

    // Start-Zeichen '*' zeichnen
    const char* pattern = start_stop_pattern;
    for (int i = 0; i < 9; ++i) {
        int width = (pattern[i] == '1') ? WIDE : NARROW;
        if (i % 2 == 0) { // Dies ist ein Balken
            p += sprintf(p, "  <rect x=\"%d\" y=\"0\" width=\"%d\" height=\"%d\" style=\"fill:black\"/>\n", current_x, width, height);
        }
        current_x += width;
    }

    // Datenzeichen zeichnen
    for (size_t i = 0; i < num_chars; ++i) {
        // Lücke zwischen den Zeichen (eine schmale Lücke)
        current_x += NARROW;

        pattern = get_code39_pattern(text[i]);
        for (int j = 0; j < 9; ++j) {
            int width = (pattern[j] == '1') ? WIDE : NARROW;
            if (j % 2 == 0) { // Balken
                p += sprintf(p, "  <rect x=\"%d\" y=\"0\" width=\"%d\" height=\"%d\" style=\"fill:black\"/>\n", current_x, width, height);
            }
            current_x += width;
        }
    }

    // Stop-Zeichen '*' zeichnen
    current_x += NARROW; // Lücke zwischen den Zeichen
    pattern = start_stop_pattern;
    for (int i = 0; i < 9; ++i) {
        int width = (pattern[i] == '1') ? WIDE : NARROW;
        if (i % 2 == 0) { // Balken
            p += sprintf(p, "  <rect x=\"%d\" y=\"0\" width=\"%d\" height=\"%d\" style=\"fill:black\"/>\n", current_x, width, height);
        }
        current_x += width;
    }

    p += sprintf(p, "</svg>\n");
    
    // Optional: Speicher neu zuweisen, um genau zu passen.
    char* final_svg = (char*)realloc(svg, strlen(svg) + 1);
    if(final_svg == NULL){
        fprintf(stderr, "Fehler: Neuzuweisung des Speichers fehlgeschlagen.\n");
        free(svg);
        return NULL;
    }

    return final_svg;
}

#endif // BARCODE_IMPLEMENTATION
#endif // BARCODE_H