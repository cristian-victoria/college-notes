import sqlite3
from contextlib import contextmanager

DB_PATH = "guild.db"

def db_connect() -> sqlite3.Connection:
    return sqlite3.connect(DB_PATH)

# Inserts a new row into the adventurers table using the provided arguments.
def recruit_member(name, class_type, level, gold) -> None:
    conn = db_connect()

    conn.execute(
        "INSERT INTO adventurers (name, class, level, gold) VALUES (?, ?, ?, ?);",
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
    conn = db_connect()
    cursor = conn.execute(
        "SELECT name, gold FROM adventurers "
        "WHERE gold > ? "
        "ORDER BY gold DESC;",
        (min_gold,),
    )
    conn.close()

    return cursor.fetchall()

# Performs a JOIN between adventurers and quests to list which adventurers
# are qualified for which quests.
def get_eligible_quests() -> list[tuple[str, str]]:
    conn = db_connect()
    cursor = conn.execute(
        "SELECT a.name, q.title "
        "FROM adventurers AS a "
        "JOIN quests AS q ON a.level >= q.difficulty;"
    )
    conn.close()

    return cursor.fetchall()

# Groups adventurers by class type and calculates the COUNT of members and
# the AVG level for each class.
def get_class_summary() -> tuple[int, int, int]:
    conn = db_connect()
    cursor = conn.execute(
        "SELECT class, COUNT(*) AS member_count, AVG(level) AS avg_level "
        "FROM adventurers "
        "GROUP BY class "
        "ORDER BY member_count DESC;"
    )
    conn.close()

    return cursor.fetchall()

def get_all_adventurers() -> list[tuple]:
    """Return every row from the adventurers table."""
    conn = db_connect()
    try:
        cursor = conn.execute("SELECT * FROM adventurers;")
        return cursor.fetchall()
    finally:
        conn.close()


def get_all_quests() -> list[tuple]:
    """Return every row from the quests table."""
    conn = db_connect()
    try:
        cursor = conn.execute("SELECT * FROM quests;")
        return cursor.fetchall()
    finally:
        conn.close()

def main():
    get_all_adventurers()
    get_all_quests()

    return 0

if __name__ == "__main__":
    main()