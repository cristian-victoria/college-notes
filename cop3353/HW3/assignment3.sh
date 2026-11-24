#Cristian Victoria
#COP3353 - Spring 2023
#Assignment 3 Solution
#Tuesday, Apr 4, 2023


#!/b

#------- QUESTION 1 -------

cp /home/faculty/myers/ufiles/file[1-2] .
cp /home/faculty/myers/ufiles/grades .  
cp /home/faculty/myers/ufiles/t[1-2] .  

#------- QUESTION 2 -------

chmod 700 grades 
echo "========================" > output.txt
echo "Grades test 1" >> output.txt
echo "========================" >> output.txt
grades < t1 >> output.txt

echo "========================" >> output.txt
echo "Grades test 2" >> output.txt
echo "========================" >> output.txt
grades < t2 >> output.txt

#------- QUESTION 3 -------

chmod 644 file1
chmod 644 file2

#------- QUESTION 4 -------

echo "========================" >> output.txt
echo "diff test 1" >> output.txt
echo "========================" >> output.txt
diff file1 file2 >> output.txt

echo "========================" >> output.txt
echo "diff test 2" >> output.txt
echo "========================" >> output.txt
diff -b file1 file2 >> output.txt

echo "========================" >> output.txt
echo "diff test 3" >> output.txt
echo "========================" >> output.txt
diff -iw file1 file2 >> output.txt

#------- QUESTION 5 -------

echo "========================" >> output.txt
echo "grep test 1" >> output.txt
echo "========================" >> output.txt
grep -i 'the' file1 >> output.txt

echo "========================" >> output.txt
echo "grep test 2" >> output.txt
echo "========================" >> output.txt
grep -e the file1 >> output.txt

echo "========================" >> output.txt
echo "grep test 3" >> output.txt
echo "========================" >> output.txt
grep -w 'who' file1 >> output.txt

#------- QUESTION 6 -------
ps -el | grep -w 'root' > process.txt 

#------- QUESTION 7 -------
echo "========================" >> output.txt
echo "Directory Listing" >> output.txt
echo "========================" >> output.txt
ls -l . >> output.txt

#------- QUESTION 8 -------
mkdir archive
tar -cvf a3.tar file1 file2 grades output.txt process.txt
mv a3.tar archive
cd archive
gzip a3.tar

exit
