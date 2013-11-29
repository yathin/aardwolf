CREATE VIEW
    IF NOT EXISTS
        data_basic AS
            SELECT
                        p.id AS photo_id,
                        c.name AS camera_name,
                        f.name AS folder_name,
                        p.name AS photo_name,
                        p.created_on AS photo_time,
                        group_concat('"' || t.group_name || '_' || t.name || '": "' || t.value || '"') AS tags
                    FROM camera c
                    LEFT JOIN folder f ON c.id = f.camera_id
                    LEFT JOIN photo p ON p.folder_id = f.id
                    LEFT JOIN tag t ON t.photo_id = p.id
                    GROUP BY t.photo_id
                    HAVING t.photo_id IS NOT NULL;

