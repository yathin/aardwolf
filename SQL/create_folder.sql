CREATE TABLE IF NOT EXISTS folder (
    id              INTEGER     PRIMARY KEY AUTOINCREMENT,
    camera_id       INTEGER     REFERENCES camera(id) ON DELETE CASCADE,
    partition_id    INTEGER     REFERENCES partition(id) ON DELETE CASCADE,
    name            TEXT        DEFAULT "",
    created_on      DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by      INTEGER     REFERENCES user(id),
    updated_on      DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by      INTEGER     REFERENCES user(id),
    images_all      INTEGER     DEFAULT 0
);
