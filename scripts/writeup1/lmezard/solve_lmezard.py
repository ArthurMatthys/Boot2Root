import os, sys

contents = [""] * 1000
lst = dict()

a = 0
for (dirpath, dirnames, filenames) in os.walk(sys.argv[1]):
    for filename in filenames:
        new_f = dirpath + "/" + filename
        with open(new_f, 'r') as f:
            content = f.read()
            content, nbr = content.split("//file",1)
            contents[int(nbr)] = content

count = 0
code = ""
for i in contents:
    if i != "":
        code += i
        count += 1

with open ("sol.c", 'w') as f:
    f.write(code)

os.system("gcc sol.c")
os.system("./a.out")

