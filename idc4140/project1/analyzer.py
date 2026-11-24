"""
IDC 4140 - Programming Assignment 1
Wikipedia Text Mining and Document Similarity
Due 03-26-2026

All the work is done by Cristian Victoria
"""

import os
import re
import requests
from bs4 import BeautifulSoup

import nltk
from nltk.tokenize import word_tokenize
from nltk.corpus import stopwords
from nltk.stem import WordNetLemmatizer

nltk.download('punkt', quiet=True)
nltk.download('punkt_tab', quiet=True)
nltk.download('stopwords', quiet=True)
nltk.download('wordnet', quiet=True)

# Define Constants
base_url = 'https://en.wikipedia.org'
search_url = '/wiki/Chicken'
full_url = f'{base_url}{search_url}'
num_links = 20

# Outputs
raw_dir  = "documents" # raw text output folder
proc_dir    = "processed" # preprocessed text output folder
output_csv = "wikipedia_dataset.csv"
most_common_csv = "top5Words.csv"

os.makedirs(raw_dir, exist_ok=True)
os.makedirs(proc_dir,   exist_ok=True)

import pandas as pd
import collections
from collections import Counter
import numpy as np
import sklearn
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
import seaborn as sns
import matplotlib.pyplot as plt

# =====================================
# PART 1: Web Scraping
# =====================================

def access_wikipage(url) -> str:
    # Define User-Agent Header to disguise script in order to surpass Wikipedia flag
    headers = {'User-Agent': 'cjv22cBot/1.0 (cjv22c@fsu.edu) Python-requests/2.31.0'} 

    response = requests.get(url, headers=headers)

    # Display error message in case of error in accessing page
    if response.status_code != 200:
        print(f"Failed to retrieve page: {response.status_code}")
        return 1

    return response

def clean_html_content(html_content) -> str:
    # Clean text using Beautiful Soup
    soup = BeautifulSoup(html_content, 'html.parser')

    noise_selectors = [
        'table.infobox', 
        'div.navbox', 
        'div.reflist', 
        'span.mw-editsection', 
        'sup.reference', 
        'div#mw-navigation',
        'div#siteNotice',
        'footer'
    ]

    # Loop through and delete them from the "soup"
    for selector in noise_selectors:
        for element in soup.select(selector):
            element.decompose()

    # Now extract the clean text
    # separator=" " ensures words don't glue together when tags are removed
    clean_content = soup.get_text(separator=" ", strip=True)
    
    return clean_content

def save_document(text: str, filepath: str) -> None:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(text)

def scrape() -> list[dict]:
    documents: list[dict] = []

    # ------ 1. Access the Wikipedia page for Chicken
    url_response = access_wikipage(full_url)

    # ------ 2. Extract the main plain text content from the page
    html_content = url_response.text

    clean_text = clean_html_content(html_content)

    # Write plaintext to file
    name = re.sub(r"^/wiki/", "", search_url)
    filename = name + ".txt"

    # Save to "documents" folder
    raw_path = os.path.join(raw_dir, filename)
    save_document(clean_text, raw_path)

    # Append necessary information to dict
    documents.append({
        "doc_id": 0, 
        "title": name, 
        "url": full_url,
        "raw_text": clean_text, 
        "processed_text": ""
    })

    # ------ 3. Identify the first 20 valid Wikipedia article links from that page
    pattern = r'href=["\'](/wiki/[A-Za-z0-9_-]*)["\']'
    all_article_links = re.findall(pattern, html_content)
    
    # Remove any duplicates
    all_article_links = list(dict.fromkeys(all_article_links))

    # Remove '/wiki/Main_Page' and '/wiki/Chicken' (the first two articles), and keep the next 20 (num_links+3)
    twenty_links = all_article_links[2:num_links+3]

    # doc_id
    index = 1
    # ------ 4. Visit 20 linked pages
    for link in twenty_links:
        full_link = f"{base_url}{link}"
        
        link_response = access_wikipage(full_link)

        # ------ 5. Extracts the main plain text content from each linked page
        link_html_content = link_response.text
        link_clean_text = clean_html_content(link_html_content)

        # ------ 6. Stores the text from each page as a separate document
        link_name = re.sub(r"^/wiki/", "", link)
        link_filename = link_name + ".txt"

        # Save to "documents" folder
        link_raw_path = os.path.join(raw_dir, link_filename)
        save_document(link_clean_text, link_raw_path)

        # Append necessary information to dict
        documents.append({
            "doc_id": index, 
            "title": link_name, 
            "url": full_link,
            "raw_text": link_clean_text, 
            "processed_text": ""
        })
        index += 1

    return documents

# =====================================
# PART 2: Text Preprocessing
# =====================================

def preprocess(documents) -> None:
    # Preprocess all 21 documents
    for doc in documents:
        text = doc["raw_text"]

        # ------ 1. Lowercase 
        lowered = text.lower()
        # ------ 2. Tokenize
        tokens_raw = word_tokenize(lowered)
        tokens = [t for t in tokens_raw if t.isalpha()]   # drop punctuation / numbers
    
        # ------ 3. Stop-word elimination
        stop_words  = set(stopwords.words("english"))
        tokens_no_stop = [t for t in tokens if t not in stop_words]
    
        # ------ 4. Lemmatization
        lemmatizer  = WordNetLemmatizer()
        lemmas = [lemmatizer.lemmatize(t) for t in tokens_no_stop] # final clean tokens

        preprocessed_text = " ".join(lemmas)

        filename = doc["title"] + "_processed.txt"

        # Save to "processed" folder
        proc_path = os.path.join(proc_dir, filename)
        save_document(preprocessed_text, proc_path)

        # Add it to list of documents
        doc["processed_text"] = preprocessed_text

# =====================================
# PART 3: Build a dataset
# =====================================

def build_dataset(documents) -> pd.DataFrame:
    df = pd.DataFrame(documents, columns=[
        "doc_id", 
        "title", 
        "url",
        "raw_text", 
        "processed_text",
    ])

    return df

# =====================================
# PART 4: Most Common Words
# =====================================

def most_common_words(df: pd.DataFrame) -> pd.DataFrame:
    rows = []
    
    for _, doc in df.iterrows():
        # Tokenize each string
        tokens = doc["processed_text"].split()

        # Get the top 5 most common words
        top5 = Counter(tokens).most_common(5)

        row = {
            "document_id": doc["doc_id"],
            "title": doc["title"]
        }

        for num, (word, freq) in enumerate(top5, start=1):
            row[f"Word {num}"]  = word
            row[f"Frequency {num}"]  = freq
 
        rows.append(row)

    df_most_common = pd.DataFrame(rows)

    return df_most_common

# =====================================
# PART 5: TF-IDF Similarity Analysis
# =====================================

def compute_similarity_analysis(df: pd.DataFrame) -> np.ndarray:
    titles = df["title"].tolist()
    entire_texts = df["processed_text"].tolist()

    vectorizer = TfidfVectorizer()
    tfidf_matrix = vectorizer.fit_transform(entire_texts)

    # ------ 1. Compute pairwise document similarity using cosine similarity
    # ------ 2. Create a 21 x 21 similarity matrix
    similarity_matrix = cosine_similarity(tfidf_matrix)

    # ------ 3. Interpret which documents appear most similar to one another
    # keep a list of simialrity score and titles of document pairs
    pairs = []
    for i in range(len(titles)):
        for j in range(i + 1, len(titles)):
            pairs.append((similarity_matrix[i, j], titles[i], titles[j]))
    
    # sort by score (descending)
    pairs.sort(reverse=True)

    # Print results
    print("The following are the top 10 most similar documents:\n")
    print(f"{'Rank'}\t{'Similarity'}\t{'Document A'}\t{'Document B'}")
    for rank, (score, doc_a, doc_b) in enumerate(pairs[:10], start=1):
        print(f"{rank}\t{score:.4f}\t{doc_a}\t{doc_b}")

    return similarity_matrix

# =====================================
# PART 6: Visualization
# =====================================

def visualizations(similarity_matrix) -> None:
    sns.heatmap(similarity_matrix, annot=False, cmap='YlGnBu')
    plt.title('Cosine similarity', fontsize=16, pad=20)
    plt.ylabel('Document A', fontsize=12)
    plt.xlabel('Document B', fontsize=12)
    plt.tight_layout()
    plt.savefig('cosine_sim_heatmap.png', dpi=300, bbox_inches='tight')
    plt.close()

# =====================================
# MAIN
# =====================================
def main():
    # ------ PART 1 --------
    docs = scrape()

    # ------ PART 2 --------
    preprocess(docs)

    # ------ PART 3 --------
    df = build_dataset(docs)
    df.to_csv(output_csv, index=False, encoding="utf-8")

    df_dataset = pd.read_csv(output_csv)

    # ------ PART 4 --------
    df_most_common = most_common_words(df_dataset)
    df_most_common.to_csv(most_common_csv, index=False, encoding="utf-8")

    # ------ PART 5 --------
    sim_matrix = compute_similarity_analysis(df_dataset)

    # ------ PART 6 --------
    visualizations(sim_matrix)

    return 0

if __name__ == "__main__":
    main()