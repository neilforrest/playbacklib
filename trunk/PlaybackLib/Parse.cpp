#include "stdafx.h"
#include <math.h>

// Convert string to float
bool StringToFloat ( char* str, float* flt )
{
	// Count the number of digits before the d.p.
	int prefixCount= 0;

	// Length of whole string
	int strLength= strlen ( str );

	// Initialise answer to 0
	(*flt)= 0;

	bool reverseSign= false;
	int readFrom= 0;

	// Check for minus sign
	if ( strLength >= 1 && str[0] == '-' )
	{
		reverseSign= true;
		readFrom= 1;
	}

	// For each char in str
	int i;
	for ( i= readFrom; i < strLength; i++ )
	{
		// If char is not the d.p.
		if ( str[i] != '.' )
		{
			// Icr count
			prefixCount++;
		}
		else
		{
			// char is d.p. finnished counting
			break;
		}
	}

	// Encountered dp. yet?
	bool decimal= false;

	// Current power of 10
	int power= prefixCount-1;

	// For each char in str
	for ( i= readFrom; i < strLength; i++ )
	{
		// If char is valid
		if ( isdigit ( str[i] ) || // Char is a number, OR
			((!decimal)&&(str[i]=='.')) )// Decimal not encountered AND char is decimal
		{
			// If char is decimal, record this
			if ( str[i] == '.' )
			{
				decimal= true;
			}
			else	// char is a digit
			{
				// Int value of char
				int digit= str[i] - '0';

				// Add to the answer
				(*flt)+= (float) ( ((float)digit) * pow ( (double)10, power ) );

				// decriment the power of 10
				power--;
			}
		}
		else // invalid char
		{
			return false;
		}
	}

	// reverse sign
	if ( reverseSign )
	{
		(*flt)= 0 - (*flt);
	}

	// Success
	return true;
}

// Convert string to int
bool StringToInt ( char* str, int* integer )
{
	// Count the number of digits before the d.p.
	int prefixCount= 0;

	// Length of whole string
	int strLength= strlen ( str );

	// Initialise answer to 0
	(*integer)= 0;

	bool reverseSign= false;
	int readFrom= 0;

	// Check for minus sign
	if ( strLength >= 1 && str[0] == '-' )
	{
		reverseSign= true;
		readFrom= 1;
	}

	// For each char in str
	int i;
	for ( i= readFrom; i < strLength; i++ )
	{
		// If char is not the d.p.
		if ( str[i] != '.' )
		{
			// Icr count
			prefixCount++;
		}
		else
		{
			// char is d.p. finnished counting
			break;
		}
	}

	// Current power of 10
	int power= prefixCount-1;

	// For each char in str
	for ( i= readFrom; i < strLength; i++ )
	{
		// If char is valid
		if ( isdigit ( str[i] ) )
		{
			// Int value of char
			int digit= str[i] - '0';

			// Add to the answer
			(*integer)+= (int) ( ((float)digit) * pow ( (double)10, power ) );

			// decriment the power of 10
			power--;
		}
		else // invalid char
		{
			return false;
		}
	}

	// reverse sign
	if ( reverseSign )
	{
		(*integer)= 0 - (*integer);
	}

	// Success
	return true;
}