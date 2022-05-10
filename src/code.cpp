#include <tice.h>
#include <graphx.h>
#include <stdlib.h>
#include <debug.h>
#include "wordlist.h"

#define TEXT_SCALE  2
#define TEXT_WIDTH  8 * TEXT_SCALE
#define TEXT_HEIGHT 8 * TEXT_SCALE


enum Colour {
	BG = 0,
	OUTLINE,
	TEXT,
	GREY,
	YELLOW,
	GREEN,
	KB_TEXT_DEFAULT,
	KB_TEXT_UPDATED = TEXT,
	KB_DEFAULT,
	KB_GREY = GREY,
	KB_YELLOW = YELLOW,
	KB_GREEN = GREEN,
};

enum Hint {
	NONE,
	ABSENT = Colour::GREY,
	PRESENT = Colour::YELLOW,
	CORRECT = Colour::GREEN,
};

inline constexpr void word_from_index(char* out, uint16_t index)
{
	// Copy the word to out
	uint24_t bitcount = index * WORD_SIZE * LETTER_SIZE;
	uint24_t start = bitcount / 8;
	uint8_t offset = bitcount % 8;
	
	for (uint8_t i = 0; i < WORD_SIZE; i++) {
		uint8_t letter = 0;
		for (uint8_t j = 0; j < LETTER_SIZE; j++) {
			uint8_t bitpos = offset + i * LETTER_SIZE + j;
			uint8_t byte = WORDLIST[start + bitpos / 8];
			size_t bitstate = (byte >> (7 - (bitpos % 8))) & 1;
			letter |= bitstate << (LETTER_SIZE - j - 1);
		}
		out[i] = letter + 'a';
	}

	out[WORD_SIZE] = '\0';
}	

inline void random_answer(char* out)
{
	// Choose a random word
	size_t index = (rand() % ANSWERS_END);

	word_from_index(out, index);
}

inline void print_coloured(char* word, Hint* colour)
{
	uint16_t x = gfx_GetTextX(), y = gfx_GetTextY();
	for (size_t i = 0; i < WORD_SIZE; i++) {
		gfx_SetTextBGColor(colour[i]);
		gfx_SetColor(colour[i]);
		gfx_FillRectangle(x + i * TEXT_WIDTH, y, TEXT_WIDTH, TEXT_HEIGHT);
		gfx_PrintChar(word[i]);
	}
	gfx_SetTextBGColor(Colour::BG);
	gfx_SetColor(Colour::BG);
}

inline void get_feedback(char* secret, char* guess, Hint* out)
{
	for (uint8_t i = 0; i < WORD_SIZE; i++)
		if (secret[i] == guess[i])
			out[i] = Hint::CORRECT;
		else
			out[i] = Hint::ABSENT;
	
	for (uint8_t i = 0; i < WORD_SIZE; i++)
		for (uint8_t j = 0; j < WORD_SIZE; j++)
			if (out[j] == Hint::ABSENT && out[i] != Hint::CORRECT && secret[i] == guess[j]) {
				out[j] =  Hint::PRESENT;
				break;
			}
}

inline int8_t cmp(char* word1, char* word2)
{
	// Compare the words alphabetically
	for (size_t i = 0; i < WORD_SIZE; i++) {
		if (word1[i] < word2[i])
			return -1;
		else if (word1[i] > word2[i])
			return 1;
	}
	return 0;
}

inline bool binary_search(char* search, size_t start, size_t end)
{
	char word[WORD_SIZE + 1];
	while (start < end) {
		size_t mid = (start + end) / 2;
		word_from_index(word, mid);
		int8_t cmp_result = cmp(word, search);
		if (cmp_result == 0)
			return true;
		else if (cmp_result > 0)
			end = mid;
		else
			start = mid + 1;
	}
	return false;
}

inline bool validate_guess(char* guess)
{
	// The answer list is sorted individually from the guess list, so we must search them individually
	return binary_search(guess, 0, ANSWERS_END) || binary_search(guess, ANSWERS_END, WORDLIST_END);
}

inline void get_guess(char* out)
{
	out[WORD_SIZE] = '\0';
	const char *chars = "\0\0\0\0\0\0\0\0\0\0\0wrmh\0\0\0\0vqlg\0\0\0zupkfc\0\0ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
	uint8_t i = 0;

	while (true) {
		auto key = os_GetCSC();
		if (key == sk_Enter && i == WORD_SIZE && validate_guess(out))
			break;

		if (key == sk_Del && i > 0) {
			i--;
			gfx_SetTextXY(
				gfx_GetTextX() - TEXT_WIDTH,
				gfx_GetTextY()
			);
			gfx_SetColor(Colour::BG);
			gfx_FillRectangle(
				gfx_GetTextX(), gfx_GetTextY(), 
				TEXT_WIDTH, TEXT_HEIGHT
			);
			continue;
		}

		if (i == WORD_SIZE) continue;

		if (chars[key]) {
			out[i++] = chars[key];
			gfx_PrintChar(chars[key]);
		}
	}
}

inline void init_theme(int theme)
{	
	/*
		Here be magic numbers
	          ,  ,
	           \\ \\
	           ) \\ \\    _p_
	           )^\))\))  /  *\
	            \_|| || / /^`-'
	   __       -\ \\--/ /
	 <'  \\___/   ___. )'
	      `====\ )___/\\
	           //     `"
	           \\    /  \
	           `"
	*/

	if (theme == 1) {
		gfx_palette[Colour::BG]              = gfx_RGBTo1555(0x12, 0x12, 0x13);
		gfx_palette[Colour::OUTLINE]         = gfx_RGBTo1555(0x3a, 0x3a, 0x3c);
		gfx_palette[Colour::TEXT]            = gfx_RGBTo1555(0xff, 0xff, 0xff);
		gfx_palette[Colour::GREY]            = gfx_RGBTo1555(0x3a, 0x3a, 0x3c);
		gfx_palette[Colour::YELLOW]          = gfx_RGBTo1555(0xb5, 0x9f, 0x3b);
		gfx_palette[Colour::GREEN]           = gfx_RGBTo1555(0x53, 0x8d, 0x4e);
		gfx_palette[Colour::KB_TEXT_DEFAULT] = gfx_RGBTo1555(0xff, 0xff, 0xff);
		gfx_palette[Colour::KB_DEFAULT]      = gfx_RGBTo1555(0x81, 0x83, 0x84);
	} else {
		gfx_palette[Colour::BG]              = gfx_RGBTo1555(0xff, 0xff, 0xff);
		gfx_palette[Colour::OUTLINE]         = gfx_RGBTo1555(0xd3, 0xd6, 0xda);
		gfx_palette[Colour::TEXT]            = gfx_RGBTo1555(0xff, 0xff, 0xff);
		gfx_palette[Colour::GREY]            = gfx_RGBTo1555(0xd3, 0xd6, 0xda);
		gfx_palette[Colour::YELLOW]          = gfx_RGBTo1555(0xc9, 0xb4, 0x58);
		gfx_palette[Colour::GREEN]           = gfx_RGBTo1555(0x6a, 0xaa, 0x64);
		gfx_palette[Colour::KB_TEXT_DEFAULT] = gfx_RGBTo1555(0x00, 0x00, 0x00);
		gfx_palette[Colour::KB_DEFAULT]      = gfx_RGBTo1555(0xd3, 0xd6, 0xda);
	}
}

inline void init_game()
{
	srand(rtc_Time());
	gfx_Begin();

	init_theme(1);

	gfx_FillScreen(Colour::BG);

	gfx_SetTextScale(TEXT_SCALE, TEXT_SCALE);
	gfx_SetMonospaceFont(8);
	gfx_SetTextTransparentColor(Colour::BG);
	gfx_SetTextBGColor(Colour::BG);
	gfx_SetTextFGColor(Colour::TEXT);
}

int main() 
{
	init_game();

	char secret_word[WORD_SIZE + 1];
	random_answer(secret_word);
	
	char user_word[WORD_SIZE + 1];
	for (uint8_t i = 0; i < 6; i++) {
		gfx_SetTextXY(0, i * TEXT_HEIGHT);
		get_guess(user_word);

		Hint hints[WORD_SIZE];
		get_feedback(secret_word, user_word, hints);

		gfx_SetTextXY(0, gfx_GetTextY());
		print_coloured(user_word, hints);
		if (cmp(secret_word, user_word) == 0)
			break;
	}
	gfx_SetTextXY(0, gfx_GetTextY() + TEXT_HEIGHT);
	gfx_PrintString("The word was ");
	gfx_PrintString(secret_word);
	gfx_PrintString("!");

	while (!os_GetCSC());
	
	gfx_End();
	return 0;
}