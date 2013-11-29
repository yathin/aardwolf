CREATE TABLE IF NOT EXISTS partition (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    name        TEXT        DEFAULT "-NA-",
    location    TEXT        DEFAULT "/",
    active      INTEGER     DEFAULT 1,
    created_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by  INTEGER     REFERENCES user(id),
    updated_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by  INTEGER     REFERENCES user(id)
);
