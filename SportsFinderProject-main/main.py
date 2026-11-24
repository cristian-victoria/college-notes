from flask import Flask, render_template, request, redirect, url_for, flash, session, jsonify
import sqlite3
import os

app = Flask(__name__)
app.secret_key = os.environ.get('SECRET_KEY', 'dev')


def get_db_connection():
    conn = sqlite3.connect('database.db')
    conn.row_factory = sqlite3.Row
    return conn


@app.route('/')
def home():
    if 'username' in session:
        return render_template('home.html', username=session['username'])
    flash('Please log in to access this page.')
    return redirect(url_for('login'))


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['login_password']

        conn = get_db_connection()
        user = conn.execute('''
            SELECT * FROM User WHERE name = ? AND login_password = ?
        ''', (username, password)).fetchone()
        conn.close()

        if user:
            session['username'] = user['name']
            return redirect(url_for('home'))
        else:
            flash('Invalid login credentials.')
            return redirect(url_for('login'))
    return render_template('login.html')

@app.route('/add_user', methods=['GET', 'POST'])
def add_user():
    if request.method == 'POST':
        name = request.form['name']
        age = request.form['age']
        email = request.form['email']
        login_password = request.form['login_password']

        errors = []
        if not name or not age or not email or not login_password:
            errors.append("All fields are required.")
        if not age.isdigit():
            errors.append("Age must be an integer.")

        if errors:
            return render_template('add_user.html', errors=errors, name=name, age=age,
                                   email=email, login_password=login_password)

        conn = get_db_connection()
        conn.execute('''
            INSERT INTO User (name, age, email, login_password)
            VALUES (?, ?, ?, ?)
        ''', (name, age, email, login_password))
        conn.commit()
        conn.close()

        flash('User added successfully!')
        return redirect(url_for('home'))

    return render_template('add_user.html')



@app.route('/list_users')
def list_users():
    if 'username' not in session:
        flash('Please log in to access this page.')
        return redirect(url_for('login'))

    conn = get_db_connection()
    users = conn.execute('SELECT name, age, email FROM User').fetchall()
    conn.close()

    return render_template('list_users.html', users=users)



@app.route('/logout')
def logout():
    session.clear()
    flash('You have been logged out.')
    return redirect(url_for('login'))


@app.route('/add_entry', methods=['GET', 'POST'])
def add_entry():
    if request.method == 'POST':
        sport = request.form['sport']
        name = request.form['name']
        latitude = float(request.form['latitude'])
        longitude = float(request.form['longitude'])

        conn = get_db_connection()
        conn.execute('''
            INSERT INTO Locations (sport, location_name, latitude, longitude)
            VALUES (?, ?, ?, ?)
        ''', (sport, name, latitude, longitude))
        conn.commit()
        conn.close()
        flash('Location added successfully!')
        return redirect(url_for('map'))

    return render_template('add_entry.html')
@app.route('/reviews')
def reviews():
    if 'username' not in session:
        flash('Please log in to access this page.')
        return redirect(url_for('login'))

    conn = get_db_connection()
    user = conn.execute('SELECT id FROM User WHERE name = ?', (session['username'],)).fetchone()

    reviews = conn.execute('''
        SELECT Reviews.review_id, Reviews.stars, Reviews.review_text,
               Reviews.user_id, User.name AS user_name,
               Locations.location_name AS location_name
        FROM Reviews
        JOIN User ON Reviews.user_id = User.id
        JOIN Locations ON Reviews.entry_id = Locations.id
        ORDER BY Reviews.review_id DESC
    ''').fetchall()

    conn.close()

    return render_template('reviews.html', reviews=reviews, user_id=user['id'])

@app.route('/add_review', methods=['GET', 'POST'])
def add_review():
    if 'username' not in session:
        flash('You must be logged in to add a review.')
        return redirect(url_for('login'))

    conn = get_db_connection()
    locations = conn.execute('SELECT id, location_name FROM Locations').fetchall()
    user = conn.execute('SELECT id FROM User WHERE name = ?', (session['username'],)).fetchone()

    if request.method == 'POST':
        entry_id = request.form['entry_id']
        stars = request.form['stars']
        review_text = request.form['review_text']

        errors = []
        if not stars.isdigit() or not (1 <= int(stars) <= 5):
            errors.append("Stars must be a number between 1 and 5.")
        if not entry_id:
            errors.append("Location must be selected.")

        if errors:
            return render_template('add_review.html', errors=errors, games=locations)

        conn.execute('''
            INSERT INTO Reviews (user_id, entry_id, stars, review_text)
            VALUES (?, ?, ?, ?)
        ''', (user['id'], entry_id, stars, review_text))
        conn.commit()
        conn.close()

        flash("Review submitted successfully!")
        return redirect(url_for('home'))

    return render_template('add_review.html', games=locations)

@app.route('/delete_review/<int:review_id>', methods=['POST'])
def delete_review(review_id):
    if 'username' not in session:
        flash('Please log in to perform this action.')
        return redirect(url_for('login'))

    conn = get_db_connection()
    user = conn.execute('SELECT id FROM User WHERE name = ?', (session['username'],)).fetchone()

    review = conn.execute('SELECT * FROM Reviews WHERE review_id = ?', (review_id,)).fetchone()

    if review and review['user_id'] == user['id']:
        conn.execute('DELETE FROM Reviews WHERE review_id = ?', (review_id,))
        conn.commit()
        flash('Review deleted successfully.')
    else:
        flash('You are not authorized to delete this review.')

    conn.close()
    return redirect(url_for('reviews'))


@app.route('/map')
def map():
    return render_template('map.html')


@app.route('/api/locations')
def api_locations():
    conn = get_db_connection()
    rows = conn.execute('SELECT sport, location_name, latitude, longitude FROM Locations').fetchall()
    conn.close()

    locations = []
    for row in rows:
        locations.append({
            'type': row['sport'],
            'name': row['location_name'],
            'coords': [row['latitude'], row['longitude']]
        })

    return jsonify(locations)


if __name__ == "__main__":
    app.run(debug=True)
