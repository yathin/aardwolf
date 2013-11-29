CREATE TABLE IF NOT EXISTS photo (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    folder_id   INTEGER     REFERENCES folder(id) ON DELETE CASCADE,
    name        TEXT        DEFAULT "",
    created_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by  INTEGER     REFERENCES user(id),
    updated_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by  INTEGER     REFERENCES user(id)
);
