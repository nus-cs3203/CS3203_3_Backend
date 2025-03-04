# How to set up dummmy data?

1. Install mongosh: https://www.mongodb.com/docs/mongodb-shell/install/
2. Connect to mongodb server using mongosh:
```bash
    mongosh
```
3. Create a database called `CS3203`:
```bash
    use CS3203;
```
4. Insert dummy data:
```bash
    db.complaints.insertMany([
    {
        "title": "RIP JB Jeyaretnam. Possibly Singapore's greatest citizen.",
        "description": "This is a description for complaint #1",
        "id": "ab123",
        "url": "https://example.com/",
        "sentiment": 0.477567863,
        "category": "Politics",
        "source": "Reddit",
        "date": new Date("2008-09-30")
    },
    {
        "title": "Good article on the Singapore economy: 'Review strategy, take crisis as opportunity '",
        "description": "This is a description for complaint #2",
        "id": "bc234",
        "url": "https://example.com/",
        "sentiment": -0.287195891,
        "category": "Transportation",
        "source": "Reddit",
        "date": new Date("2008-10-22")
    },
    {
        "title": "High-living Singaporean monk faces jail for fraud.",
        "description": "This is a description for complaint #3",
        "id": "cd345",
        "url": "https://example.com/",
        "sentiment": -0.475928083,
        "category": "Infrastructure",
        "source": "Reddit",
        "date": new Date("2009-10-09")
    },
    {
        "title": "The Harold Lloyd Method of Mass Transit Advertising",
        "description": "This is a description for complaint #4",
        "id": "de456",
        "url": "https://example.com/",
        "sentiment": 0.837125612,
        "category": "Financial",
        "source": "Reddit",
        "date": new Date("2009-11-17")
    },
    {
        "title": "Hello la, I'm in Singapore for the week and want to go dancing, was wondering Zirca or Zouk, and which days?",
        "description": "This is a description for complaint #5",
        "id": "ef567",
        "url": "https://example.com/",
        "sentiment": -0.175837054,
        "category": "Housing",
        "source": "Reddit",
        "date": new Date("2010-01-12")
    },
    {
        "title": "A Curious Case of Censorship?",
        "description": "This is a description for complaint #6",
        "id": "fg678",
        "url": "https://example.com/",
        "sentiment": 0.267879895,
        "category": "Public Safety",
        "source": "Reddit",
        "date": new Date("2010-03-28")
    },
    {
        "title": "/r/Singapore: I'm thinking of producing a war/soldier-themed short-film. However, I'm torn between the general lack of support for it from peers or ideas.",
        "description": "This is a description for complaint #7",
        "id": "gh789",
        "url": "https://example.com/",
        "sentiment": -0.000265525,
        "category": "Healthcare",
        "source": "Reddit",
        "date": new Date("2010-05-03")
    },
    {
        "title": "A Golden Investment That Sounds Too Good To Be True",
        "description": "This is a description for complaint #8",
        "id": "hi890",
        "url": "https://example.com/",
        "sentiment": -0.0809918,
        "category": "Retail",
        "source": "Reddit",
        "date": new Date("2010-05-04")
    },
    {
        "title": "Singapore dog mill goes bust, 75 dogs used for breeding need to be rehomed.",
        "description": "This is a description for complaint #9",
        "id": "ij901",
        "url": "https://example.com/",
        "sentiment": 0.500866343,
        "category": "Environmental",
        "source": "Reddit",
        "date": new Date("2010-05-06")
    },
    {
        "title": "A Singapore Startup",
        "description": "This is a description for complaint #10",
        "id": "jk012",
        "url": "https://example.com/",
        "sentiment": 0.264428436,
        "category": "Housing",
        "source": "Reddit",
        "date": new Date("2010-05-07")
    },
    {
        "title": "Interview with Singaporean Crust-Grind band Wormrot.",
        "description": "This is a description for complaint #11",
        "id": "kl123",
        "url": "https://example.com/",
        "sentiment": 0.761470821,
        "category": "Healthcare",
        "source": "Reddit",
        "date": new Date("2010-05-09")
    },
    {
        "title": "Singapore Users: Anyone else having problems loading images from imgur.com? Seems like Singtel users are affected",
        "description": "This is a description for complaint #12",
        "id": "lm234",
        "url": "https://example.com/",
        "sentiment": 0.245745575,
        "category": "Environmental",
        "source": "Reddit",
        "date": new Date("2010-06-21")
    },
    {
        "title": "I can't access imgur link from starhub and singnet",
        "description": "This is a description for complaint #13",
        "id": "mn345",
        "url": "https://example.com/",
        "sentiment": 0.270746942,
        "category": "Transportation",
        "source": "Reddit",
        "date": new Date("2010-06-21")
    },
    {
        "title": "Solution to imgur.com for Singtel users..",
        "description": "This is a description for complaint #14",
        "id": "no456",
        "url": "https://example.com/",
        "sentiment": -0.436532214,
        "category": "Social Services",
        "source": "Reddit",
        "date": new Date("2010-06-22")
    },
    {
        "title": "Big drain not working",
        "description": "This is a description for complaint #15",
        "id": "op567",
        "url": "https://example.com/",
        "sentiment": -0.911377971,
        "category": "Recreation",
        "source": "Reddit",
        "date": new Date("2010-07-17")
    },
    {
        "title": "Singapore's suicide rate rising (at least 401 in 2009)",
        "description": "This is a description for complaint #16",
        "id": "pq678",
        "url": "https://example.com/",
        "sentiment": 0.756529545,
        "category": "Recreation",
        "source": "Reddit",
        "date": new Date("2010-07-26")
    },
    {
        "title": "Does anyone outside of Singapore even know or care that the Youth Olympic Games is going on now?",
        "description": "This is a description for complaint #17",
        "id": "qr789",
        "url": "https://example.com/",
        "sentiment": -0.73751096,
        "category": "Financial",
        "source": "Reddit",
        "date": new Date("2010-08-13")
    },
    {
        "title": "So.. we're kinda like STOMP but much more civilised right?",
        "description": "This is a description for complaint #18",
        "id": "rs890",
        "url": "https://example.com/",
        "sentiment": -0.198254379,
        "category": "Politics",
        "source": "Reddit",
        "date": new Date("2010-08-20")
    },
    {
        "title": "Preparing For A.C.M.E II and FAQs",
        "description": "This is a description for complaint #19",
        "id": "st901",
        "url": "https://example.com/",
        "sentiment": -0.215923078,
        "category": "Housing",
        "source": "Reddit",
        "date": new Date("2010-08-25")
    },
    {
        "title": "Rewarding our foreign workers for their hard work",
        "description": "This is a description for complaint #20",
        "id": "tu012",
        "url": "https://example.com/",
        "sentiment": 0.696234156,
        "category": "Employment",
        "source": "Reddit",
        "date": new Date("2010-09-03")
    }
    ]);
```

Note: Take note of the mongodb server you are connected to. Turn off your local mongodb server if you plan to use docker mongodb server.