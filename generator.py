import random
num = 200000;
letters = list('abcdefghijklmnopqrstuvwxyz')

file = open("huge.sql","w")

file.write("create table huge (aa int, bb float, cc char(1), primary key(aa));\n")
for x in xrange(0,num):
    file.write("insert into huge values(" + str(x) + ',' + str(random.randint(1,1000)) + '.' + str(random.randint(1,1000))+ ",'" + letters[random.randint(0, 25)] +"');\n" )