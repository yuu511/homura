#!/usr/bin/python3
# Replace transmission credentials / plex token with your own.
import csv
import os
import subprocess
import time
import signal
import requests
import datetime
from pathlib import Path
from clutch import Client

def download():
  client = Client(address="http://localhost:9091/transmission/rpc",username='transmission',password='transmission')
  torrents_to_check = []
  csv_file = os.path.join (os.path.dirname(os.path.realpath(__file__)),'shows_to_download.csv')

  with open (csv_file ,newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
      search_term = row['Search_Term']
      save_dest = row['Save_Destination']
      proc = subprocess.Popen( ['homura', '--torrents_only', '--delay_end', 'search', search_term], stdout=subprocess.PIPE )
      output = subprocess.check_output((['head','-n1']),stdin=proc.stdout)
      proc.wait()
      torrent_magnet = output.decode("utf-8")
      torrent_info = client.torrent.add({"filename":torrent_magnet, "download_dir":save_dest})      
      if torrent_info.arguments.torrent_added:
        print("Torrent %s added at %s" % (torrent_info.arguments.torrent_added.name,datetime.datetime.now()))
        torrents_to_check.append(torrent_info.arguments.torrent_added.id)

    still_downloading = True
    if torrents_to_check:
      while still_downloading:
        still_downloading = False
        for id in torrents_to_check:
          torrent_info = client.torrent.accessor(fields=['percent_done','name'],ids=id)  
          if torrent_info.result and torrent_info.result=='success':
            if ((torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['percent_done']) != 1.0):
              still_downloading = True
            else:
              print ("Torrent %s completed at %s" % (torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['name'],datetime.datetime.now()))
              break
        if still_downloading:
          time.sleep(30)
      requests.get('http://localhost:32400/library/sections/1/refresh?X-Plex-Token=__YOURTOKENHERE__')

if __name__ == "__main__":
    print ("Script started at %s." % datetime.datetime.now()) 
    download()
