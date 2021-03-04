# IOT Project

### Setup conda env

<!-- ```
conda create -n iot python=3
conda activate iot
conda install -c conda-forge fastapi
conda install -c conda-forge uvicorn
conda install -c conda-forge opencv=4.1.0
``` -->

```
pip3 install fastapi uvicorn websockets opencv=4.1.0
```

# Body/face detection

```
python detect.py
```

# Websockets

```
uvicorn api:api --reload
```
