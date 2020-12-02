#!/usr/bin/python3
# Replace transmission credentials / plex token with your own.
import csv
import os
import subprocess
import time
import signal
import requests
import datetime
import getopt,sys
from pathlib import Path
from clutch import Client

def scrape_nyaasi(client):
  csv_file = os.path.join (os.path.dirname(os.path.realpath(__file__)),'shows_to_download_kuro.csv')
  torrents_to_check = []

  with open (csv_file ,newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
      search_term = row['Search_Term']
      save_dest = row['Save_Destination']
      torrent_magnet = None
      output = None
      proc = subprocess.Popen( ['homura', '--torrents_only', '--delay_end', 'search', search_term], stdout=subprocess.PIPE )
      if download_all:
        output = proc.communicate()
        if output and output[0]:
          torrent_magnet = output[0].decode("utf-8")
          magnets = torrent_magnet.splitlines()
          if not magnets:
            continue
          for magnet in magnets:
            torrent_info = client.torrent.add({"filename":magnet, "download_dir":save_dest})      
            if torrent_info.arguments.torrent_added:
              print("Torrent %s added at %s" % (torrent_info.arguments.torrent_added.name,datetime.datetime.now()))
              torrents_to_check.append(torrent_info.arguments.torrent_added.id)
      else:
        output = subprocess.check_output((['head','-n1']),stdin=proc.stdout)
        proc.wait()
        torrent_magnet = output.decode("utf-8")
        if not torrent_magnet:
          continue
        torrent_info = client.torrent.add({"filename":torrent_magnet, "download_dir":save_dest})      
        if torrent_info.arguments.torrent_added:
          print("Torrent %s added at %s" % (torrent_info.arguments.torrent_added.name,datetime.datetime.now()))
          torrents_to_check.append(torrent_info.arguments.torrent_added.id)
  return torrents_to_check

def wait_download_complete(client, torrents_to_check):
  still_downloading = True
  if torrents_to_check:
    while still_downloading:
      still_downloading = False
      for id in torrents_to_check:
        torrent_info = client.torrent.accessor(fields=['percent_done','name'],ids=id)  
        if torrent_info.result and torrent_info.result=='success':
          percent_done = torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['percent_done']
          name = torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['name']
          if (percent_done != 1.0):
            still_downloading = True
          else:
            print ("Torrent %s completed at %s" % (name, datetime.datetime.now()))
            continue
      if still_downloading:
        time.sleep(30)

def plexhooks():
  requests.get('http://localhost:32400/library/sections/1/refresh?X-Plex-Token=__YOURTOKENHERE__')

def download(download_all):
  client = Client(address="http://localhost:9091/transmission/rpc",username='transmission',password='transmission')
  torrents_to_check = scrape_nyaasi(client)
  if torrents_to_check:
    wait_download_complete(client, torrents_to_check)
    plexhooks()

if __name__ == "__main__":
    print ("Script started at %s." % datetime.datetime.now()) 
    try:
      opts, args = getopt.getopt(sys.argv[1:], "a", ["download_all"])
    except getopt.GetoptError as err:
      sys.exit(2)
    download_all = False
    for o,a in opts:
      if o == "-a":
        download_all = True
    download(download_all)
    print ("Script completed at %s." % datetime.datetime.now()) 
