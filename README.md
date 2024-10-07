# plc-esp
Programmable Logic Controller for ESP microcontrollers
<br>
<img src="https://raw.githubusercontent.com/Denisov-Foundation-Limited/plc-esp/develop/img/net.png" width=300 />
<img src="https://raw.githubusercontent.com/Denisov-Foundation-Limited/plc-esp/develop/img/socket.png" width=300 />
<br>
<img src="https://raw.githubusercontent.com/Denisov-Foundation-Limited/plc-esp/develop/img/cli.png" width=500 />
<img src="https://raw.githubusercontent.com/Denisov-Foundation-Limited/plc-esp/develop/img/tgbot.png" width=300 />
<br>
<img src="https://raw.githubusercontent.com/Denisov-Foundation-Limited/plc-esp/develop/img/board.png" width=700 />
<br>
<img src="https://raw.githubusercontent.com/Denisov-Foundation-Limited/plc-esp/develop/img/ext.png" width=700 />

## API

### Socket

#### GET
```
http://192.168.0.8:8080/ctrl?name=Розетки
http://192.168.0.8:8080/ctrl?name=Розетки&socket=Свитч1
```
#### SET
```
http://192.168.0.8:8080/ctrl?name=Розетки&socket=Свитч1&status=true
```
