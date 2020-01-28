# find-replace
Copies input file to output file with the specified keys being replaced with their paired replacementStrings

input file - input (no extension)
key file - keys.txt
output file - output (no extension) (note that this file is overwritten if it exists)

key-replacement string pairs are stored in a keys.txt file
The first line of the file contains the number of key-replacement pairs.
Each line represents the pair. The first word of the line is the key and the remaining of the line is the string to replace it with.
If no replacement string is given for a specified key, the key is essentially deleted in the output file.
If there is ambiguity in which key to use, the one that appears first in the file is used.