CREATE VIEW
    IF NOT EXISTS
        folder_tagged AS
            SELECT
                f.id AS folder_id,
                COUNT(DISTINCT t.photo_id) AS images_done
                FROM
                    folder f
                    LEFT JOIN photo p ON p.folder_id = f.id
                    LEFT JOIN tag t ON t.photo_id = p.id
                WHERE t.photo_id IS NOT NULL
                GROUP BY f.id;
