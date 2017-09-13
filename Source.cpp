// The encryption program in C++ and ASM with a very simple encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
// Filename: "4473 FoCA 2016 Encryption Original with ASM.cpp"
//
// Harry Walker, last update: Apr 2016
// Student no: 25044102

#include <conio.h>		// for kbhit
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
using namespace std;

#define MAXCHARS 6		// feel free to alter this, but 6 is the minimum
#define dollarchar '$'  // string terminator

char OChars[MAXCHARS],
EChars[MAXCHARS],
DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char(char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{
		cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars(int& length)
{
	char next_char;
	length = 0;
	get_char(next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars[length++] = next_char;
		get_char(next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars(int length, char EKey)
{
	char temp_char;						// Character temporary store
	int index(0);

	//for (int i = 0; i < length; i++)	// Encrypt characters one at a time
	//{

		_asm {

			; ************************** This part is the FOR loop *************************

				mov			index, 0				
				jmp			checkSize
	forloop:	mov			eax, index
				add			eax, 1
				mov			index, eax

			; ************************** This part is the CHECK *************************

	checkSize:  cmp			index, 6
				jge			endFor

			; ************************** This part is the BODY *************************

		// TO DO -> temp_char = OChars[i];			// Get the next char from Original Chars array

				push		eax						//push	eax onto the parameter stack		
				push		ecx						//push ecx onto the parameter stack						

				movzx		ecx, temp_char			//move the contents of temp_char into ECX register with zero extends
				lea			eax, EKey				//load the contents of the EKey into the EAX register

				push		ecx
				push		eax

				call		encrypt17

				add			esp, 8					//Clean the stack

				mov			temp_char, al

				pop			ecx
				pop			eax					//push eax off the parameter stack

				jmp			forloop				//jump back to the for loop label 
			
			   movzx		al, temp_char
			   lea			ecx, EChars + [ebx]
			   mov			byte ptr[ecx], al

			; ************************** This part is the END of the forloop *************************

		   endFor:

			  // EChars[i] = temp_char;
		}

	//}

	return;


	// Encrypt subroutine. You should paste in the encryption routine you've been allocated from Bb and
	// overwrite this initial, simple, version. Ensure you change the ‘call’ above to use the
	// correct 'encryptnn' label where nn is your encryption routine number.
	// Inputs: register EAX = 32-bit address of Ekey,
	//					ECX = the character to be encrypted (in the low 8-bit field, CL).
	// Output: register EAX = the encrypted value of the source character (in the low 8-bit field, AL).
	__asm {

	encrypt17:
		push		ebp							//allow us to use parameter stack
			mov			ebp, esp					//copy the ESP value into the EBP

			mov			eax, [EBP + 8]
			mov			ecx, [EBP + 12]

			push		esi						//Initilaise ESI register onto the stack
			push		ecx						//Push the character to be encrypted onto the stack

			mov			esi, eax				//Copy the contents of ESI into EAX

			and			dword ptr[esi], 0xFF	//And together random value of ESI with 0xFF hex (254)
			ror			byte ptr[esi], 1		//Rotate right one bit ESI	
			ror			byte ptr[esi], 1		//Rotate right one bit ESI
			add			byte ptr[esi], 0x01		//Add one to ESI value	
			mov			ecx, [esi]				//The character to be encrypted = ESI register that has been changed
			pop			edx						//Pop off edx off the stack

		x17 : ror			dl, 1					//x17 label and rotate right dl by one
			  dec			ecx						//Decrement the value by one 
			  jnz			x17						//Jump back to x17 if ECX is not zero
			  mov			eax, edx				//Move the contents of EDX into the encrypted value
			  add			eax, 0x20				//Add 0x20 hex (32) to the encrypted value
			  xor			eax, 0xAA				//Xor the encrypted by 0xAA hex (170) 

			  pop			esi						//Pop ESI (temp char) off the stack

			  pop			ebp						//pop the original value of EBP back into EBP from the stack
			  ret									//Return for CDECLS

	}

	//--- End of Assembly code
}
// end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars(int length, char EKey)
{
	char temp_char;									// Character temporary store

	for (int i = 0; i < length; i++)				// Encrypt characters one at a time
	{
		temp_char = EChars[i];						// Get the next char from Original Chars array

		__asm {										// Start of assembly code

				push		eax						// Push	EAX onto the parameter stack, EAX = 0 in binary		
				push		ecx						// Push ECX onto the parameter stack		

				movzx		ecx, temp_char			// Popy the contents of the temp_char into the ECX register with ZERO extends
				lea			eax, EKey				// Load the contents of the Ekey into the EAX register

				push		ecx
				push		eax

				call		decrypt17				// Call the decrypt subroutine

				add			esp, 8					// Clean the stack

				mov			temp_char, dl			// Was AL 8bit register previously; chan

				pop			ecx						// Pop off and restore the register for use in the future 
				pop			eax						// Pop off and restore the register for use in the future 			
		}

		DChars[i] = temp_char;						// Store encrypted char in the Encrypted Chars array
	}

	return;

	_asm {
	decrypt17:
			push		ebp							// Push EBP register onto the stack to reference function parameters
			mov			ebp, esp					// Popy the ESP value into the EBP register

			mov			eax, [EBP + 8]				// Base pointer on the stack being copied into Parameter being mapped to EAX register (E-key) Space needs to be made
			mov			ecx, [EBP + 12]				// Parameter being mapped to the ECX register (character to be enctypted) from the EBP stack

			push		esi							// Initilaise ESI register onto the stack
			push		ecx							// Push the character to be encrypted onto the stack

			mov			esi, eax					// Copy the contents of ESI into EAX
			and			dword ptr[esi], 0xFF		// And together random value of ESI with 0xFF hex (254)
			ror			byte ptr[esi], 1			// Rotate right one bit ESI	
			ror			byte ptr[esi], 1			// Rotate right one bit ESI
			add			byte ptr[esi], 0x01			// Add one to ESI value	
			mov			ecx, [esi]					// The character to be encrypted = ESI register that has been changed
			pop			edx							// Pop off EDX off the stack

			mov			eax, edx					// Move the contents of EDX into the encrypted value

			xor			eax, 0xAA					// XOR again to decrypt previously XORed Ekey
			sub			eax, 0x20					// Subtract 0x20 (32) from the EAX register

			mov			edx, eax					// Move the contents of the EAX register into the EDX 32-bit general register

		x17 : rol			dl, 1						// x17 label and rotate LEFT (instead of right before) DL by one
			  dec			ecx							// Decrement the ECX register by 1
			  jnz			x17							// Jump back to x17 if ECX is not zero
			  pop			esi							// Pop ESI (temp char) off the stack and push it back to the caller as the Ekey
			  pop			ebp							// Pop the original value of EBP back into EBP from the stack
			  ret										// Return to the calling function 
	}


}
// end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------

int main(void)
{
	int  char_count;	// The number of actual characters entered (upto MAXCHARS limit).
	char EKey;			// Encryption key.

	cout << "\nPlease enter your Encryption Key (EKey) letter: "; get_char(EKey);

	cout << "\nNow enter upto " << MAXCHARS << " alphanumeric characters:\n";
	get_original_chars(char_count);
	cout << "\n\nOriginal source string = " << OChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << hex << setw(2) << setfill('0') << ((int(OChars[i])) & 0xFF) << "  ";

	encrypt_chars(char_count, EKey);
	cout << "\n\nEncrypted string       = " << EChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << ((int(EChars[i])) & 0xFF) << "  ";

	decrypt_chars(char_count, EKey);
	cout << "\n\nDecrypted string       = " << DChars << "\tHex = ";
	for (int i = 0; i<char_count; i++) cout << ((int(DChars[i])) & 0xFF) << "  ";

	cout << "\n\nPress a key to end...";
	while (!_kbhit());					//hold the screen until a key is pressed
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------

