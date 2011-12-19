/*
	TEST_PREGEN.C
	-------------
*/

#include <cstdio>
#include <cassert>

#include "../source/string_pair.h"
#include "../source/pregen.h"
#include "../source/pregen_writer_normal.h"

/*
	MAIN()
	------
*/
int main(void)
{
/* Check that leading spaces are not significant, and verify that the encoding of a string of
 * spaces (the smallest codepoint in the base37 encoding) doesn't generate an RSV of zero.
 */
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("")) == 1);
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair(" ")) == 1);
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("  ")) == 1);
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("  a"))
		== ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("a")));

/* Check that punctuation is correctly discarded by the restrictive base37 model */
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("a'!a@$^a"))
		== ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("aaa")));
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair(" a'! a@  $^ a"))
		== ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("a a a")));

assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("river packs WOLF RP (accepting! New)")) > 1);

//Check that invalid UTF-8 sequences do not derail encoding
assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("\xa4 Zhyna en Icacia [wolf rp] [semi lit+] [open]")) > 1
		&& ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("\xa4 Zhyna en Icacia [wolf rp] [semi lit+] [open]"))
			== ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("Zhyna en Icacia wolf rp semi lit open")));

assert(ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("\xb7 ? ? ? ? ? ? ? ? ? ? \xb7 Teenager survival RP - NEEDS MEMBERS"))
		== ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("Teenager survival RP needs MEMBERS")));

printf("%llu\n", ANT_pregen_writer_normal::generate_radix<ANT_encode_char_base37>(ANT_string_pair("۞ Ɖ α я κ  Ƭ ι м ɛ Ƨ ۞ {A humanoid/creature RP}")));

return 0;
}
