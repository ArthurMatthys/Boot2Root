def rec(i):
    if i < 2 :
        return 1
    else:
        return rec(i - 1) + rec (i - 2)


for i in range(100):
    if rec(i) == 55:
        print (i)
        break
