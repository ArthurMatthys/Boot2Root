tab = ["i", "s", "r", "v", "e", "a", "w", "h", "o", "b", "p", "n", "u", "t", "f", "g"]
other_tab = ["a","b", "c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"]

word = ["g", "i", "a", "n", "t", "s"]

for j in word:
    sol = set()
    for i in other_tab:
        a = ord(i)
        b = a & 0xf
        if tab[b] == j:
            sol.add(i)

    print("{} <-".format(j), sol)
