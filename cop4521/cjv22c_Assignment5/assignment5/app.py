from flask import Flask, render_template, request, redirect, url_for

app = Flask(__name__)

# Route 1: GET /
@app.route("/", methods=["GET"])
def home():
    return render_template("home.html")

# Route 2: GET /greet/
@app.route("/greet/<name>", methods=["GET"])
def greet(name):
    return render_template("greet.html", name=name)

# Route 3: GET /courses
@app.route("/courses", methods=["GET"])
def courses():
    course_list = ["Mathematics", "Computer Science", "Biology"]
    return render_template("courses.html", courses=course_list)

# Route 4: GET /feedback
@app.route("/feedback", methods=["GET"])
def feedback():
    return render_template("feedback.html")

# Route 5: POST /feedback
@app.route("/feedback", methods=["POST"])
def result():
    name = request.form.get("name")
    course = request.form.get("course")
    comment = request.form.get("comment")

    if not name or not course or not comment:
        return render_template("error.html")

    return render_template(
        "result.html",
        name=name,
        course=course,
        comment=comment
    )

# Route 6: GET /start
@app.route("/start", methods=["GET"])
def start():
    return redirect(url_for("home"))

if __name__ == "__main__":
    app.run(debug=True)