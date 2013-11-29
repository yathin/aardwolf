INSERT INTO reports(name, query)
    SELECT "File Count" AS name, "SELECT COUNT(DISTINCT photo.id) AS Files, COUNT(DISTINCT tag.photo_id) AS Files_Tagged FROM photo LEFT JOIN tag on tag.photo_id = photo.id" AS query
UNION SELECT  "File Count by Camera" AS name, "SELECT c.name AS Camera_Name, COUNT(DISTINCT photo.id) AS Files, COUNT(distinct tag.photo_id) AS Files_Tagged FROM camera c LEFT JOIN folder f ON f.camera_id = c.id LEFT JOIN photo ON photo.folder_id = f.id LEFT JOIN tag on tag.photo_id = photo.id GROUP BY c.id" AS query
UNION SELECT "Tag Count" AS name, "SELECT tag.group_name AS Tag_Group, tag.name AS Tag_Name, COUNT(tag.name) AS Files_Tagged FROM tag GROUP BY tag.name" AS query
;
