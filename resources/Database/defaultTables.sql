CREATE TABLE IF NOT EXISTS users (
  user_id           SERIAL NOT NULL,
  creation_date     TIMESTAMP NOT NULL DEFAULT NOW()::TIMESTAMP(0),
  name              VARCHAR(50) NOT NULL,
  username          VARCHAR(50) UNIQUE,
  pic_path          VARCHAR(255),
  PRIMARY KEY (user_id)
);

CREATE TABLE IF NOT EXISTS users_email (
  user_id           INT4 NOT NULL,
  email             VARCHAR(50) NOT NULL UNIQUE,
  password_hash     VARCHAR(255) NOT NULL,
  password_salt     VARCHAR(4) NOT NULL,
  FOREIGN KEY (user_id) REFERENCES users (user_id) DEFERRABLE INITIALLY IMMEDIATE
);

CREATE TABLE IF NOT EXISTS chats (
  chat_id SERIAL NOT NULL,
  PRIMARY KEY (chat_id)
);

CREATE TABLE IF NOT EXISTS messages_header (
  message_id          SERIAL NOT NULL,
  from_user_id        INT4 NOT NULL,
  to_chat_id          INT4 NOT NULL,
  send_time           TIMESTAMP NOT NULL,
  reply_to_message_id INT4 NOT NULL,
  PRIMARY KEY (message_id),
  FOREIGN KEY (from_user_id) REFERENCES users (user_id),
  FOREIGN KEY (to_chat_id) REFERENCES chats (chat_id),
  FOREIGN KEY (reply_to_message_id) REFERENCES messages_header (message_id)
);

CREATE TABLE IF NOT EXISTS messages_body (
  message_id        INT4 NOT NULL UNIQUE,
  text              INT4 NOT NULL,
  FOREIGN KEY (message_id) REFERENCES messages_header (message_id)
);

CREATE TABLE IF NOT EXISTS users_in_chats (
  user_id int4 NOT NULL,
  chat_id int4 NOT NULL,
  FOREIGN KEY (user_id) REFERENCES users (user_id),
  FOREIGN KEY (chat_id) REFERENCES chats (chat_id)
);
