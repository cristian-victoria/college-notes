import sqlite3
import requests
from bs4 import BeautifulSoup

URL = "https://www.talgov.com/parks/parks-meadows?utm_source=chatgpt.com"

def initialize_database():
    conn = sqlite3.connect('database.db')

    # Create User table
    conn.execute('''
    CREATE TABLE IF NOT EXISTS User (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        age INTEGER NOT NULL,
        email TEXT NOT NULL,
        login_password TEXT NOT NULL
    )
    ''')

    # Create Locations table
    conn.execute('''
    CREATE TABLE IF NOT EXISTS Locations (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        sport TEXT NOT NULL,
        location_name TEXT NOT NULL,
        latitude REAL NOT NULL,
        longitude REAL NOT NULL
    )
    ''')

    # Create Reviews table
    conn.execute('''
    CREATE TABLE IF NOT EXISTS Reviews (
        review_id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_id INTEGER NOT NULL,
        entry_id INTEGER NOT NULL,
        review_text TEXT,
        stars INTEGER CHECK (stars >= 1 AND stars <= 5),
        FOREIGN KEY (user_id) REFERENCES User(id),
        FOREIGN KEY (entry_id) REFERENCES Locations(id)
    )
    ''')

    # Insert sample user data
    accounts = [
        ("Erick Reynoso", 23, "test1@gmail.com", "erickpass"),
        ("Alex Castonan", 23, "test2@gmail.com", "alexpass"),
        ("Cassandra Mori", 33, "test3@gmail.com", "cassandrapass"),
    ]
    conn.executemany('''
        INSERT INTO User (name, age, email, login_password)
        VALUES (?, ?, ?, ?)
    ''', accounts)

    # Insert sample location data
    sample_locations = [
        ("soccer", "Tom Brown Park", 30.459, -84.217),
        ("soccer", "Main Campus Fields", 30.4382, -84.2921),
        ("soccer", "Rec SportsPlex", 30.4082, -84.3524),
        ("soccer", "Seminole Soccer Complex", 30.4398, -84.3087),
        ("soccer", "Meadows Soccer Complex", 30.5082, -84.2503),
        ("basketball", "Jack McLean Park", 30.411, -84.291),
        ("basketball", "Westside Courts", 30.4435, -84.3050),
        ("basketball", "Basketball Training Facility", 30.4383, -84.2925),
        ("basketball", "Leach Recreation Center", 30.4408, -84.2992),
        ("basketball", "Optimist Park", 30.4262, -84.2629),
        ("football", "Doak Campbell Stadium", 30.438, -84.304),
        ("football", "Doak S. Campbell Stadium", 30.4381, -84.3040),
        ("football", "Gene Cox Stadium", 30.4123, -84.2679),
        ("football", "Bragg Memorial Stadium", 30.4214, -84.2857),
        ("football", "Florida High Stadium", 30.3775, -84.2205),
    ]
    conn.executemany('''
        INSERT INTO Locations (sport, location_name, latitude, longitude)
        VALUES (?, ?, ?, ?)
    ''', sample_locations)

    # Insert sample reviews
    sample_reviews = [
        (1, 1, "Great field! Super clean and organized.", 5),
        (2, 3, "Gets really busy on weekends but still a nice spot.", 4),
        (3, 6, "Courts need maintenance but fun atmosphere.", 3),
        (1, 11, "Football field is well-kept. Loved the vibe.", 5),
        (2, 10, "Decent spot, parking can be a pain.", 4)
    ]

    conn.executemany('''
        INSERT INTO Reviews (user_id, entry_id, review_text, stars)
        VALUES (?, ?, ?, ?)
    ''', sample_reviews)


    conn.commit()
    conn.close()
    print("Database initialized successfully!")


def scrape_and_insert_fields():
    conn = sqlite3.connect("database.db")
    c = conn.cursor()

    response = requests.get(URL)
    soup = BeautifulSoup(response.text, "html.parser")

    for item in soup.find_all("div", class_="location"):
        name = item.find("h3").text.strip()

        # Placeholder coordinates for now
        latitude = 30.4383
        longitude = -84.2807

        c.execute('''
            INSERT INTO Locations (sport, location_name, latitude, longitude)
            VALUES (?, ?, ?, ?)
        ''', ("soccer", name, latitude, longitude))

    conn.commit()
    conn.close()
    print("Scraped soccer fields added to database!")


if __name__ == "__main__":
    initialize_database()
    scrape_and_insert_fields()
