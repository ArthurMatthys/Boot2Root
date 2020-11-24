import itertools
import subprocess
import os

sols = "Public speaking is very easy.\n1 2 6 24 120 720\n2 b 755\n9\nopekma\n"

answers = set()
answers_link = dict()

for i in itertools.permutations("12356"):
    new_sol = sols + "4 "
    for j in i:
        new_sol += "{} ".format(j)
    with open("sol.txt", "w") as f:
        f.write(new_sol)
    os.system('./bomb < sol.txt > tmp.txt')
    with open("tmp.txt", "r") as f:
        l1 = len(answers)
        r = f.read()
        answers.add(r)
        l2 = len(answers)
        if l1 != l2:
            answers_link[r] = new_sol
    if os.path.exists("tmp.txt"):
        os.remove("tmp.txt")
    if os.path.exists("sol.txt"):
        os.remove("sol.txt")

for i in answers_link.keys():
    print(answers_link[i], "\n", i)
