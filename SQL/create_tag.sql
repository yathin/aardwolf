CREATE TABLE IF NOT EXISTS tag (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    photo_id    INTEGER     REFERENCES photo(id) ON DELETE CASCADE,
    group_name  TEXT        DEFAULT "NA",
    name        TEXT        DEFAULT "NA",
    value       TEXT        DEFAULT "NA",
    created_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by  INTEGER     REFERENCES user(id),
    updated_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by  INTEGER     REFERENCES user(id)
);
