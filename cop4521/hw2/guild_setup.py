import sqlite3
from contextlib import contextmanager

# Establish data base connection
@contextmanager
def get_connection(db_path: str):
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    
    yield conn
    
    conn.close()

def clean_slate(conn: sqlite3.Connection) -> None:
    # Start with clean slate and 2. Schema definitions
    cursor = conn.cursor()
    
    cursor.executescript("""
        DROP TABLE IF EXISTS adventurers;
        DROP TABLE IF EXISTS quests;
                                
        CREATE TABLE adventurers (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            class_type TEXT NOT NULL,
            level INTEGER Default 1,
            gold INTEGER
        );

        CREATE TABLE quests (
            quest_id INTEGER PRIMARY KEY,
            title TEXT NOT NULL,
            difficulty INTEGER,
            reward_gold INTEGER
        );
    """)

    conn.commit()

def load_data(conn: sqlite3.Connection, file_path: str) -> None:
    cursor = conn.cursor()
    
    with open(file_path, "r") as f:
        for line in f:
            # Skip blank lines
            if not line:
                continue
                
            parts = []
            for p in line.split(","):
                parts.append(p.strip())

            table_type = parts[0].lower()

            if table_type == "adventurer":
                # adventurer, Name, Class, Level, Gold
                _, name, cls, level, gold = parts
                cursor.execute(
                    "INSERT INTO adventurers (name, class_type, level, gold) "
                    "VALUES (?, ?, ?, ?);",
                    (name, cls, int(level), int(gold)),
                )
            elif table_type == "quest":
                # quest, Title, Difficulty, Reward
                _, title, difficulty, reward = parts
                cursor.execute(
                    "INSERT INTO quests (title, difficulty, reward_gold) "
                    "VALUES (?, ?, ?);",
                    (title, difficulty, int(reward)),
                )

    conn.commit()

def main():
    db_path = "guild.db"
    textfile = "guild_data.txt"

    # 1. Establish connection with database
    with get_connection(db_path) as conn:

        # 2. Schema definitions
        clean_slate(conn)

        # 3. Data Import
        load_data(conn, textfile)

    return 0

if __name__ == "__main__":
    main()