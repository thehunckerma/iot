from fastapi import FastAPI, WebSocket
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import time

api = FastAPI()

api.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@api.websocket("/ws/motor")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    # while True:
    #     time.sleep(1)
    #     # data = await websocket.receive_text()
    #     await websocket.send_text("0")
    time.sleep(10)
    await websocket.send_text("180")


@api.websocket("/ws/camera")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    while True:
        data = await websocket.receive_bytes()
        f = open('image.jpeg', 'w+b')
        binary_format = bytearray(data)
        f.write(binary_format)
        f.close()
        print("bruh")


if __name__ == '__main__':
    uvicorn.run(api, host='0.0.0.0', port=8080)
