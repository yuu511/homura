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
import torch
import gc
import whisper
from whisper.utils import get_writer
from pathlib import Path
from clutch import Client

def scrape_nyaasi(client):
  csv_file = os.path.join (os.path.dirname(os.path.realpath(__file__)),'shows_to_download_kuro.csv')
  torrents_to_check = []


  with open (csv_file ,newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    base_path = "/home/elijah/anime3/アニメ/"
    copy_path = "/home/elijah/anime3/plex_アニメ/"
    for row in reader:
      search_term = row['Search_Term']
      save_dest = row['Save_Destination']
      renamefmt = row['OPTIONAL_rename']
      whisper = row['OPTIONAL_whisper']

      basedir = os.path.join(base_path,save_dest)
      copydir = os.path.join(copy_path,save_dest)

      if (not os.path.exists(basedir)):
        print ("Made directory %s" % basedir)
        os.mkdir(basedir)

      if (not os.path.exists(copydir)):
        print ("Made directory %s" % copydir)
        os.mkdir(copydir)

      tuple = {}
      
      torrent_magnet = None
      output = None
      proc = subprocess.Popen( ['homura', '--refresh_cache', '--torrents_only', '--delay_end', 'search', search_term], stdout=subprocess.PIPE )
      if download_all:
        output = proc.communicate()
        if output and output[0]:
          torrent_magnet = output[0].decode("utf-8")
          magnets = torrent_magnet.splitlines()
          if not magnets:
            continue
          for magnet in magnets:
            tuple = {}
            torrent_info = client.torrent.add({"filename":magnet, "download_dir":str(basedir)})      
            if torrent_info.arguments.torrent_added:
              print("Torrent %s added at %s" % (torrent_info.arguments.torrent_added.name,datetime.datetime.now()))
              tuple["torrent_id"] = torrent_info.arguments.torrent_added.id

              if (copydir != ""):
                tuple["copy_dir"] = str(copydir)
              else:
                tuple["copy_dir"] = None

              if (renamefmt != ""):
                tuple["rename_fmt"] = renamefmt
              else:
                tuple["rename_fmt"] = None

              if (whisper != ""):
                tuple["whisper"] = whisper
              else:
                tuple["whisper"] = None

              torrents_to_check.append(tuple)
      else:
        output = subprocess.check_output((['head','-n1']),stdin=proc.stdout)
        proc.wait()
        torrent_magnet = output.decode("utf-8")
        if not torrent_magnet:
          continue
        torrent_info = client.torrent.add({"filename":torrent_magnet, "download_dir":str(basedir)})      
        if torrent_info.arguments.torrent_added:
          print("Torrent %s added at %s" % (torrent_info.arguments.torrent_added.name,datetime.datetime.now()))

          tuple["torrent_id"] = torrent_info.arguments.torrent_added.id

          if (copydir != ""):
            tuple["copy_dir"] = str(copydir)
          else:
           tuple["copy_dir"] = None

          if (renamefmt != ""):
            tuple["rename_fmt"] = renamefmt
          else:
            tuple["rename_fmt"] = None

          if (whisper != ""):
            tuple["whisper"] = whisper
          else:
            tuple["whisper"] = None

          torrents_to_check.append(tuple)

  return torrents_to_check

def wait_download_complete(client, torrents_to_check,model):
  if torrents_to_check:
    i = 0
    while torrents_to_check:  
      tuple = torrents_to_check[i]
      id = tuple["torrent_id"]
      torrent_info = client.torrent.accessor(fields=['percent_done','name','files','download_dir'],ids=id)  
      if torrent_info.result and torrent_info.result=='success':
        percent_done = torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['percent_done']
        name = torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['name']
        download_dir = torrent_info.dict(exclude_none=True)["arguments"]["torrents"][0]['download_dir']
        if (percent_done != 1.0):
          i +=1
          if (torrents_to_check and i == (len(torrents_to_check))):
            i = 0
            time.sleep(30)
        else:
          fullpath = os.path.join(download_dir,name)
          print ("Torrent %s completed at %s" % (name, datetime.datetime.now()))
          print (tuple["copy_dir"])
          if (tuple["copy_dir"] != None):
            proc = subprocess.Popen( ['cp', '-a', '-l','-v', fullpath , tuple["copy_dir"]], stdout=subprocess.PIPE )
            output = proc.communicate()
            if (proc.returncode == 0):
              # file_format = 's/\[.*\][_ *](.*)[_\- ]+ *(\d+).*(\.\w+)/$1 s01e$2$3/'
              fullpath = os.path.join(tuple["copy_dir"],name)

              if (tuple["whisper"] != None):
                print("Creating subs by whisper")
                result = model.transcribe(fullpath, language="ja",condition_on_previous_text=False)
                writer = get_writer("srt", tuple["copy_dir"])
                writer(result, fullpath)

              if (tuple["rename_fmt"] != None):
                print ("Linking to %s" % fullpath)
                print ("rename_fmt %s" % tuple["rename_fmt"])

                # proc = subprocess.Popen( ['rename', '-n', tuple["rename_fmt"], fullpath], stdout=subprocess.PIPE )
                # output = proc.communicate()
                # print (output)
                # proc = subprocess.Popen( ['rename', tuple["rename_fmt"], fullpath], stdout=subprocess.PIPE )
                # output = proc.communicate()

                # fk this, fuck python. this is how you get it to work....
                os.chdir(tuple["copy_dir"])
                cmdstring = "rename " + tuple["rename_fmt"] + " " + "*.mkv"
                print ( "%s" % cmdstring)
                os.system(cmdstring)
                cmdstring = "rename " + tuple["rename_fmt"] + " " + "*.mp4"
                print ( "%s" % cmdstring)
                os.system(cmdstring)
                cmdstring = "rename " + tuple["rename_fmt"] + " " + "*.srt"
                print ( "%s" % cmdstring)
                os.system(cmdstring)

          del torrents_to_check[i] 
          if (torrents_to_check and i >= len(torrents_to_check)):
            i = 0

def plexhooks():
  requests.get('http://localhost:32400/library/sections/1/refresh?X-Plex-Token=__YOURTOKENHERE__')

def download(download_all):
  client = Client(address="http://localhost:9091/transmission/rpc",username='transmission',password='transmission')
  torrents_to_check = scrape_nyaasi(client)
  if torrents_to_check:
    model = whisper.load_model("medium")
    wait_download_complete(client, torrents_to_check,model)
    # whisper delete cache
    del model
    torch.cuda.empty_cache()
    #
    gc.collect()
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
