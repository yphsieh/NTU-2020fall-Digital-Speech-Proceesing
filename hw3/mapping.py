import sys

def main(input, output):
    zy2big5 = {}
    with open(input, 'r', encoding='cp950') as file:
        for line in file:
            big5, zhuyin = line.split(' ')
            zhuyin = zhuyin.split('/')
            for zy in zhuyin:
                if zy[0] in zy2big5.keys(): 
                    zy2big5[zy[0]].append(big5)
                else:
                    zy2big5[zy[0]] = [big5]
            
    with open(output, 'w', encoding='cp950') as file:
        for zhuyin in sorted(zy2big5.keys()):
            file.write(zhuyin)
            for big5 in zy2big5[zhuyin]:
                file.write(' {}'.format(big5))
            file.write('\n')

            for big5 in zy2big5[zhuyin]:
                file.write('{} {}\n'.format(big5, big5))
            

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
