#Python program to read
# json file
  
  
import json
import re
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
# Opening JSON file
f = open('dataset.json',)
stop_words = set(stopwords.words('english'))
# returns JSON object as 
# a dictionary
data = []
allabs = open("allabstracts100.txt", "a")
r = 0
for line in open('dataset.json', 'r'):
    r +=1
    data.append(json.loads(line))
    if r == 100:
        break;

for i in range(100):
    text = data[i]['abstract']
    text = text.lower()
    text = re.sub(r'\d+','',text)
    text = re.sub(r'[^\w\s]','', text)
    text_tokens = word_tokenize(text)
    for w in text_tokens:
        if w not in stop_words:
            allabs.write(w + " ")
    allabs.write("\n")
    allabs.write(text)
    allabs.write("$$\n")


    
    
# Closing file
f.close()
allabs.close()
