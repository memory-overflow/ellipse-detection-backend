from cgitb import text
import os
import sys
import time
import base64
import json
import hashlib
import binascii
import datetime
import hmac
import requests

def unicode_convert(input):
    if isinstance(input, dict):
        return {unicode_convert(key): unicode_convert(value) for key, value in input.iteritems()}
    elif isinstance(input, list):
        return [unicode_convert(element) for element in input]
    elif isinstance(input, str):
        return input.encode('utf-8')
    else:
        return input

def image_to_base64(image_path):
    with open(image_path, 'rb') as f:
        base64_str = base64.b64encode(f.read()).decode()
        return base64_str

def base64_to_image(base64_str):
  return base64.b64decode(base64_str)


def main():
  if len(sys.argv) <= 1:
    exit(0)
  filepath = sys.argv[1]
  bas64 = image_to_base64(filepath)
  body = {
    "RequestId": "test",
    # "Image": bas64, 
    "ImageUrl" : "https://7072-prod-4gam91yoc446aa31-1309268318.tcb.qcloud.la/images/x3rDCRHMTS8yTT4W"
  }
  url = "http://127.0.0.1:8000/EllipseDetect"
  response = requests.post(url, json=body)
  print(response.status_code)
  if response.status_code == 200:
    data = json.loads(response.text)
    if not 'ErrorCode' in data:
      image = data['ShowImage']
      f = open('test.jpg', 'wb')
      f.write(base64_to_image(image))
      f.close()
  else:
    print("Faile: ", response.status_code)

main()