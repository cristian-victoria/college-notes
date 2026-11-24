import sqlite3
from contextlib import contextmanager

DB_PATH = "guild.db"

def db_connect() -> sqlite3.Connection:
    return sqlite3.connect(DB_PATH)

# Inserts a new row into the adventurers table using the provided arguments.
def recruit_member(name, class_type, level, gold) -> None:
    conn = db_connect()

    conn.execute(
        "INSERT INTO adventurers (name, class_type, level, gold) VALUES (?, ?, ?, ?);",
        (name, class_type, level, gold),
    )
    
    conn.commit()

    conn.close()

# Updates the level of the adventurer with the specified member id.
def update_member_level(member_id, new_level) -> None:
    conn = db_connect()
    conn.execute(
        "UPDATE adventurers SET level = ? WHERE id = ?;",
        (new_level, member_id),
    )
    conn.commit()
    conn.close()

# Deletes the adventurer with the specified member id from the database.
def banish_member(member_id) -> None:
    conn = db_connect()
    conn.execute(
        "DELETE FROM adventurers WHERE id = ?;",
        (member_id,),
    )
    conn.commit()
    conn.close()

# Selects the name and gold (only these two columns) of adventurers who have 
# gold strictly greater than (>) the min gold argument.
def get_wealthy_adventures(min_gold) -> list[tuple[str, int]]:
    try:
        conn = db_connect()
        cursor = conn.execute(
            "SELECT name, gold FROM adventurers "
            "WHERE gold > ? "
            "ORDER BY gold DESC;",
            (min_gold,),
        )
        return cursor.fetchall()
    finally: # Ensures connection is closed after return
        conn.close()

# Performs a JOIN between adventurers and quests to list which adventurers
# are qualified for which quests.
def get_eligible_quests() -> list[tuple[str, str]]:
    try:
        conn = db_connect()
        cursor = conn.execute(
            "SELECT a.name, q.title "
            "FROM adventurers AS a "
            "JOIN quests AS q ON a.level >= q.difficulty;"
        )
        conn.close()
    finally: # Ensures connection is closed after return
        conn.close()

# Groups adventurers by class_type and calculates the COUNT of members and
# the AVG level for each class_type.
def get_class_summary() -> tuple[int, int, int]:
    try:
        conn = db_connect()
        cursor = conn.execute(
            "SELECT class_type, COUNT(*) AS member_count, AVG(level) AS avg_level "
            "FROM adventurers "
            "GROUP BY class_type "
            "ORDER BY member_count DESC;"
        )
        return cursor.fetchall()
    finally:
        conn.close()