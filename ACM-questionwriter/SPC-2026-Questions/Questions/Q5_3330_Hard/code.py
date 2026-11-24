'''To run: gc in1.txt | python cop3330med.py'''
class Astronaut:
    def __init__(self, name, oxygen_level, distance):
        self.name = name
        self.oxygen_level = oxygen_level
        self.distance = distance
    

def parseLines(totalAstro):
    num = int(input().strip())   #grabs number of lines

    for i in range (0,num): #reads in and populates the array
        line = input()
        name, oLevel, dist  = line.split(',')   #parse line
        totalAstro.append(Astronaut(name,int(oLevel),int(dist)))  #init array


def sortArray(totalAstro):
    for i in range(len(totalAstro)-1):    #sorting algo (bubble sort)
        posMin = i
        for j in range(i+1, len(totalAstro)):
            if totalAstro[j].oxygen_level < totalAstro[posMin].oxygen_level:
                posMin = j
            
            elif totalAstro[j].oxygen_level == totalAstro[posMin].oxygen_level: #custom: if the oxygen levels are equal, sort by distance, smallest first
                if totalAstro[j].distance < totalAstro[posMin].distance:
                    posMin = j

        temp = totalAstro[i]
        totalAstro[i] = totalAstro[posMin]
        totalAstro[posMin] = temp

if __name__ == "__main__":
    
    totalAstro = []

    #grab the lines and parse them for class init
    parseLines(totalAstro)

    #now put everything in ascending order
    sortArray(totalAstro)
    
    #print
    print("CRITICAL:")
    for astro in totalAstro:
        if astro.oxygen_level <= 30:
            print(astro.oxygen_level,"%: ",astro.name,", Distance: ",astro.distance,"m", sep='')
        
        
    print("\nNON-CRITICAL:")
    for astro in totalAstro:
        if astro.oxygen_level > 30:
            print(astro.oxygen_level,"%: ",astro.name,", Distance: ",astro.distance,"m", sep='')
