 function WebSocketTest() {
                alert(getFullName(person));

                if ("WebSocket" in window) {
                    // Let us open a web socket
                    var ws = new WebSocket("ws://localhost:12345/echo_thrice");

                    ws.onerror = function() {
                        alert("error: " + ws);
                    };

                    ws.onopen = function() {
                        // Web Socket is connected, send data using send()
                        ws.send("Hello Server");
                    };

                    ws.onmessage = function (evt) {
                        var received_msg = evt.data;
                        alert("Message is received: " + received_msg);
                    };

                    ws.onclose = function() {
                        // websocket is closed.
                        alert("Connection is closed...");
                    };
                }
                else {
                    // The browser doesn't support WebSocket
                    alert("WebSocket NOT supported by your Browser!");
                }
            }