CREATE TABLE IF NOT EXISTS project (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT    DEFAULT "-NA-",
    location        TEXT    DEFAULT "-NA-",
    info            TEXT    DEFAULT "-NA-",
    xmp             INTEGER DEFAULT 0
);
