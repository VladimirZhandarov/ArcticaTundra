SELECT pg_catalog.setval('"FilterSection_id_seq"', 7, true);

INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (1, 'root', 'root', 0, NULL);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (2, '��', '������� �������������', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (3, '��', '������������ �������', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (4, '��', '������� ��������', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (5, '���', '��������������� ������� �������������', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (6, '���', '��������������� ������������ �������', 1, 1);
INSERT INTO "FilterSection"(id, name, description, level, "idParentSection") VALUES (7, '��', '��������� �������', 1, 1);
