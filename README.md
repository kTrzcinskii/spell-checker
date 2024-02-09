# Spell Checker

## Introduction
Spell Checker is command line program written in C that checks if the word is correct (according to the provided dictionary). In case when the word can be found in the dictionary then it's considered to be correct. Otherwise, spell checker goes through dictionary and find words which are closest to the provided word. Levenshtein distance is used for calculating distance between two words.

The main goal of this project was to practice data structures (specifically red-black trees) and to learn more about Levenshtein distance and how to calculate it properly and quickly.

## Usage

For very basic usage you can enter directory project and just run:
```bash
./build/spell_checker
```
Then, after every word you type(with word ending by `\n` symbol) you will be informed if word is correct or not. If not, you will be provided with number of hints (closest words available in the dictionary). Program will be running until receiving end-of-file (C-D).

Spell Checker accepts 4 cli options:
 - hints_limit
 - dictionary_path
 - input
 - ouput

#### hints_limit
hints_limits is maximum number of closest words return if provided word is not correct. Default value is 10. Minimal value is 1, maximum value is 1024. 
For example:
```bash
./build/spell_checker -l 5
```
sets hints limit to 5.

#### dictionary_path
dictionary_path is a path to the text file with words that should build the dictionary used in the program. Words in such file should be separated by `\n` symbol. Defaul value is `./assets/small_words.txt` with 10000 most popular english words, however `./assets/word.txt` is also available - it contains all of the english words, though because of the size of the file it's almost unusable.
For example
```bash
./build/spell_checker -p /path/to/custom/dictionary_file.txt
```
sets dictionary file to `/path/to/custom/dictionary_file.txt`.

#### input
input is path to the file with input words. Words in such file should be separated by `\n` symbol. If no path is provided then stdin is used as input, and words are read from it until EOF.
For example
```bash
./build/spell_checker -i ./example/input/ex1.txt
```
sets input file path to `./example/input/ex1.txt`. It's actual file in the project, so feel free to try it out.

#### output
output is path to the dictionary in which results should be put. For every word provided in the input, the file will be created at `/{output}/{word}`. The content of the file will be as follows:
```
{correctness*}
{number of hints}
{Levenshtein distance of hints}
{hints**}
```
\* 1 if word is correct, 0 otherwise
\*\* every hint is in new line

If dictionary doesn't exist then spell checker will create it before checking actual words.

If output is not provided then stdout is used, with more human-like output, which goes as follows:
If word is correct:
```
{word} is correct
```
If word is not correct:
```
{word} is not corrrect
Here are {} closest words from dictionary (with {} Levenshtein distance):
{hints}
```

For example
```bash
./build/spell_checker -o ./example/output/out1
```
will create directories `./example/output` and `./example/output/out1`. Then for every word provided in the input create file `./example/output/out1/{word}` and save results inside it.

## Compilation
Spell checker is Linux compatible. It is not tested on any other distro nor on any other OS, so there is no guarantee it will work there.

To compile program you need:
 - gcc
 - make

Enter directory with the project and run:
```bash
make all
```

## Implementation
For storing dictionary in memory I decided to use red-black trees. All of its implementation is written by me. It works well with relatively small files (with thousands of words) but when it comes to bigger files it's almost unusable. I didn't find any solution for this issue.
Calculating Levenshtein distance is implemented using dynamic programming to make it as fast as possible.

## References
- [Red-Black Trees](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree)
- [Levenshtein distance](https://en.wikipedia.org/wiki/Levenshtein_distance)
- [The Algorithm Behind Spell Checkers](https://www.youtube.com/watch?v=d-Eq6x1yssU&t=398s&ab_channel=b001) (it's a video that got me interested in the subject)