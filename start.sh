# ./peer_server \
# --ssl_srt=/etc/letsencrypt/live/selenika2022.ru/fullchain.pem \
# --ssl_key=/etc/letsencrypt/live/selenika2022.ru/privkey.pem \
# --server=selenika2022.ru --port=8080 \
# --max_num_connections=10 \
# --dbg_level=debug

./peer_server \
--ssl_srt=/home/qaz/work/config/server.crt --ssl_key=/home/qaz/work/config/server.key \
--server=localhost --port=8080 \
--max_num_connections=2 \
--dbg_level=debug

#./peer_server --ssl_srt=/home/qaz/work/config/server.crt --ssl_key=/home/qaz/work/config/server.key --server=192.168.0.106 --port=8080 --dbg_level=debug
