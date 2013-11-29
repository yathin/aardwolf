CREATE TABLE IF NOT EXISTS tag_definition (
    group_name  TEXT        DEFAULT "NA",
    name        TEXT        DEFAULT "NA",
    value_type  TEXT        DEFAULT "NA",
    shortcut    TEXT        DEFAULT "",
    xmp_name    TEXT        DEFAULT "",
    created_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by  INTEGER     REFERENCES user(id),
    updated_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by  INTEGER     REFERENCES user(id)
);
