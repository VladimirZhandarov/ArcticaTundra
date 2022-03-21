SELECT pg_catalog.setval('"FilterSection_id_seq"', 7, true);

INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (1, 'root', 'root', 0, NULL);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (2, 'ФИ', 'Фильтры интенсивности', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (3, 'ФС', 'Спектральные фильтры', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (4, 'ФО', 'Фильтры объектов', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (5, 'ФИК', 'Комбинированные фильтры интенсивности', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (6, 'ФСК', 'Комбинированные спектральные фильтры', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (7, 'ФГ', 'Групповые фильтры', 1, 1);
