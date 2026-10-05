#!/bin/bash

# list files of a directory

echo -e "1.- List directory contents.\n"

ls

echo -e "2.- List directory contents by reverse time of modification/creation.\n"

ls -lr

echo -e "3.- List files and size.\n"

ls -ls

echo -e "4.- List directory/sub-directory contents recursively.\n"

ls -R

echo -e "5.- List hidden files.\n"

ls -a

echo -e "6.- Make a new directory\n"

mkdir nombrecarpeta

echo -e "7.- Remove a directory\n"

rmdir nombrecarpeta


echo -e "8.- Remove a file\n"

rm anombrearchivo

echo -e "9.- Display directory location\n"

pwd

echo -e "10.- Dump contents of a file to users screen\n"

cat nombrearchivo

echo -e "11.- Online manuals\n"

man comando

echo -e "12.- Clear screen\n"

clear

echo -e "13.- Change file permissions\n"

chmod 744 nombrearchivo

echo -e "14.- Rename a file/ directory\n"

mv nombreantiguo nombrenuevo

echo -e "15.- Operating system/shell version\n" 

uname -r

echo -e "16.-  List command history\n"

history

echo -e "17.- Show date\n"

date

echo -e "18.-   Show mounted shares/filesystems.\n"

df -h

echo -e "19.-   Look for a word in files given in command line\n"

grep "palabra" nombrearchivo

echo -e "20.-   Compare two files and show differences.\n"

dif archivo1 archivo2

echo -e "21.-  Redirect the output of the fib.py file to an out.txt\n"

touch out.txt
python3 fib.py > out.txt

echo -e "22.-  Redirect the output of the fib.py file to the last row of out.txt\n"

python3 fib.py >> out.txt

echo -e "23.-  Use entry.txt as stdin for the fib_n.py program\n"

touch entry.txt
echo "5" > out.txt
python3 fib_n.py < entry.txt

echo -e "24.- Count the lines of fib.py file\n"

wc -l fib.py

echo -e "25.- Count the words of fib.py file\n"

wc -w fib.py

echo -e "26.- Count the characters of fib.py file\n"

wc -m fib.py

echo -e "27.- Ignore the word Linux while search the UNIX word in the example.txt (with the grep command)"

grep "UNIX" example.txt | grep -v "Linux"

echo -e "28.- How to count the occurrence of a word in the example.txt file using the grep command"

grep -o "Linux" example.txt | wc -l

echo -e "29.- Display first two lines containing UNIX in the example.txt (with grep command)"

grep "UNIX" example.txt | head -n 2

echo -e "30.-  How to search pattern using egrep and regular expression (search the word UNIX and Linux)"

egrep "UNIX|Linux" example.txt
