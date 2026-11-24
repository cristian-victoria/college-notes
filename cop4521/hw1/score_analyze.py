"""
Cristian Victoria
cjv22c
Due date: Feb 2 2026

The program in this file is the individual work of Cristian Victoria.
"""

def load_data(filename):
    """
    read student score data from text file line by line
    extract student names and scores
    validate each entry by:
        checking the file format
        removing whitespace
        confirming that scores are integers
        ensuring that scores fall within the valid range of 0 to 100
    
    skip any line that does not meet these requirements
    keep track of number of invalid lines ignored
    """

    if filename.endswith(".txt") == False:
        return "File format not supported."

    invalid_line_count = 0

    list_name_score = []
    with open(filename) as file:
        for line in file:
            no_whitespace_string = "".join(line.split())

            if "," not in no_whitespace_string:
                invalid_line_count += 1
                continue
            
            before_comma, after_comma = no_whitespace_string.split(",", 1)
            
            # Left side must not be empty
            if not before_comma:
                invalid_line_count += 1
                continue

            # Right side must be an integer
            try:
                after_comma = int(after_comma)
            except ValueError:
                invalid_line_count += 1
                continue

            # Integer must be between 0 and 100
            if after_comma < 0 or after_comma > 100:
                invalid_line_count += 1
                continue
        
            list_name_score.append(no_whitespace_string)  

    print(f"Lines ignored: {invalid_line_count}")

    return list_name_score

def letter_grade(numeric_score):
    """
    converts a numeric score into a letter grade
    takes a single integer score as input and returns a string representing the letter grade
    handle all valid scores from 0 to 100
    (A, A-, B+, B, C, D, F)
    """
    letter = "F"

    if numeric_score >= 60:
        letter = "D"
    if numeric_score >= 70:
        letter = "C"
    if numeric_score >= 80:
        letter = "B"
    if numeric_score >= 85:
        letter = "B+"
    if numeric_score >= 90:
        letter = "A-"
    if numeric_score >= 95:
        letter = "A"

    return letter

def summary_statistics(name_score_pairs_list):
    """
    analyze list of valid student scores and compute overall class statistics
    given a list of (name, score) pairs:
        determine the total number of students
        calculate the class average score
        identify the student with the highest score
        identify the student with the lowest score
        count how many students fall into each letter grade
    Computer results should be organized and returned in a dictionary
    """
    total_students = total_score = class_average = 0
    max_score_index = min_score_index = 0
    max_score = 0
    min_score = 100
    highest_scoring_student = lowest_scoring_student = ""
    index = -1
    name_list = []
    score_list = []
    lettergrade_list = []
    countA = countAminus = countBplus = countB = countC = countD = countF = 0
    for pair in name_score_pairs_list:
        index += 1
        name, score = pair.split(",", 1)
        score = int(score)

        name_list.append(name)
        score_list.append(score)
        lettergrade_list.append(letter_grade(score))
        
        total_students += 1
        total_score += score

        if score > max_score:
            max_score = score
            max_score_index = index
        if score < min_score:
            min_score = score
            min_score_index = index

    class_average = round((total_score / total_students), 2)
    highest_scoring_student = f"{name_list[max_score_index]}({score_list[max_score_index]})"
    lowest_scoring_student = f"{name_list[min_score_index]}({score_list[min_score_index]})"

    countA = lettergrade_list.count("A")
    countAminus = lettergrade_list.count("A-")
    countBplus = lettergrade_list.count("B+") 
    countB = lettergrade_list.count("B")
    countC = lettergrade_list.count("C")
    countD = lettergrade_list.count("D")
    countF = lettergrade_list.count("F")

    summary_dict = {
        "count": total_students,
        "average": class_average,
        "highest": highest_scoring_student,
        "lowest": lowest_scoring_student,
        "distribution":{"A": countA, "A-": countAminus, "B+": countBplus, "B": countB, "C": countC, "D": countD, "F": countF}
    }

    return name_list, score_list, lettergrade_list, summary_dict

def write_report_to_file(l_names: list, l_scores: list, l_letters: list, d_summary: dict):
    with open("summary.txt", "w") as outfile:
        outfile.write("Name \t Score \t Grade\n")
        for x in range(len(l_names)):
            outfile.write(f"\n{l_names[x]} \t {l_scores[x]} \t {l_letters[x]}")

        outfile.write("\nSummary\n")
        outfile.write(f"Students: {d_summary['count']}\n")
        outfile.write(f"Average: {d_summary['average']}\n")
        outfile.write(f"Highest: {d_summary['highest']}\n")
        outfile.write(f"Lowest: {d_summary['lowest']}\n")

        dist = d_summary["distribution"]
        formatted = " ".join(f"{grade}={count}" for grade, count in dist.items())
        outfile.write(f"Grade distribution: {formatted}\n")

if __name__ == "__main__":
    name_score_pairs_list = load_data("scores.txt")
    print("Finished loading data.")
    l_names, l_scores, l_letters, d_summary = summary_statistics(name_score_pairs_list)
    write_report_to_file(l_names, l_scores, l_letters, d_summary)
    print("Finished Writing report to summary.txt.")