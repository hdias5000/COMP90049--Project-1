## Project 1 - Lexical Normalisation for Twitter Data
## Hasitha Dias - 789929
## This file contains methods related to evaluation.


import json

with open("./file.json", 'r') as f:
    datastore = json.load(f)


def readfile(filename):
    f = open(filename, 'r')
    x = f.read().split('\n')
    f.close()
    return x

def eval(method):
    correct_results=0
    total_results=0
    possible_correct=0
    for matches,target,key in zip(datastore,correct,misspell):
        matched = False
        all_matches = matches[key][method]
        if target in dictionary:
            possible_correct+=1
        for match in all_matches:
            if match['match']==target:
                correct_results+=1
                matched=True
                break
        total_results+=len(all_matches)
    return (correct_results,total_results,possible_correct)


def similar(misspell,correct):
    count=0
    for miss,corr in zip(misspell,correct):
        if miss==corr:
            count+=1
    print(count)



#open all files
with open("./file.json", 'r') as f:
    datastore = json.load(f)
correct = readfile('correct.txt')[:-1]
misspell = readfile('misspell.txt')[:-1]
dictionary = readfile('dict.txt')[:-1]

#print number of words in each dataset
print("Number of Misspelled words:"+str(len(misspell)))
print("Number of Correct words:"+str(len(correct)))
print("Number of Dictionary words:"+str(len(dictionary)))


print("Statistics : ")
print(eval('edit'))
print(eval('soundex'))


(edit_correct,edit_returned,possible_correct) = eval('edit')
(soundex_correct,soundex_returned,possible_correct) = eval('soundex')


edit_precision = edit_correct/edit_returned
edit_recall = edit_correct/possible_correct

soundex_precision = soundex_correct/soundex_returned
soundex_recall = soundex_correct/possible_correct


print("Precision for Edit Dist:"+str(edit_precision))
print("Precision for Edit Dist+Soundex:"+str(soundex_precision))

print("Recall for Edit Dist:"+str(edit_recall))
print("Recall for Edit Dist+Soundex:"+str(soundex_recall))

print("Average matches for Edit Dist:"+str(edit_returned/len(correct)))
print("Average matches for Edit Dist+Soundex:"+str(soundex_returned/len(correct)))

similar(misspell,correct)
