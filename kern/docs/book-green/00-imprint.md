# Kern – Simply Explained

**The Engine, Kern Studio, a Walking Robot and the Lehrer – for Everyone Who Wants to Understand How Games and Learning Robots Work**

## Imprint and Rights

**Author, publisher and rights holder (self-published):** Christian Heinrich Hohlfeld\
Gerhart-Hauptmann-Weg 5, 78464 Konstanz, Germany\
ORCID: 0009-0003-6634-9045\
Email: chris.hohlfeld@gmail.com

ISBN {{ISBN}} (hardcover)

This is the green edition: the same content as *Kern – The Book* (the blue, technical edition),
explained more simply.

© 2026 Christian Heinrich Hohlfeld. All rights reserved. First edition 2026.\
Printed and bound on demand by Amazon (e.g. Amazon Distribution GmbH, Leipzig); the place of printing is given on the last page.

**The book.** This book and its figures are protected by copyright. Any use beyond the narrow
limits of German copyright law (Urheberrechtsgesetz, UrhG) requires the prior written consent of
the rights holder. This applies in particular to reproduction, translation, adaptation,
distribution, making available to the public, and storage and processing in electronic systems.
The rights holder expressly reserves the use of this work for training, fine-tuning or evaluating
machine-learning models (text and data mining) pursuant to § 44b(3) UrhG. The code examples in the
book may be used under the Apache License 2.0; excerpts of code ported from Pollen Robotics remain
under their Apache 2.0 license (Appendix D).

**The software.** Kern (the engine, Kern Studio, the Lehrer ("teacher"), the trained networks, tools
and examples) is open source under the Apache License 2.0 and available free of charge:
<https://kernengine.de/>, source code at
<https://github.com/ChristianHohlfeld/kernengine.de>. Excepted are the Micro Duck 3D models by
Pollen Robotics contained in Kern Studio: they are under CC BY-NC-SA 4.0 and may be used only
non-commercially. A commercial build (`kern/build.sh --commercial`) leaves them out. The names
"Kern", "Kern Studio", "Kern Lehrer" and the Kern logo are excluded from the license. Courses and
integration licenses for schools and companies are offered separately by the rights holder;
integration licenses do not include Pollen's 3D models.

**Third-party components.** Kern uses, and this book describes, Box3D (Erin Catto, MIT), Earcut
(Mapbox, ISC), musl math (MIT), techniques from Intel XeGTAO (MIT) and three.js (MIT), as well as
the Micro Duck's robot model, policies and ported software by Pollen Robotics (Apache 2.0). They
remain under their own licenses; the license texts are in [Appendix D](D-licences.md). Kern
Studio shows the duck with Pollen's 3D models (CC BY-NC-SA 4.0, non-commercial); they are neither
included nor shown in this book. The pictures of the duck in this book, including the screenshots
from Kern Studio, were made with the author's own design and do not show Pollen's 3D models.

**Pictures.** Screenshots of Kern Studio, the Lehrer and games built with Kern, all drawings and
diagrams, the Kern logo and the author's photo: © 2026 Christian Heinrich Hohlfeld.

**Trademarks.** "Micro Duck" (also "Microduck") names Pollen Robotics' (Hugging Face) robot, which
Kern simulates; the name is used only descriptively. Kern and this book are independent:
Pollen Robotics and Hugging Face have neither published, endorsed nor reviewed them. Other
trademarks and product names (among them Box3D, Unreal Engine, WebAssembly, WebGL, ONNX, GitHub,
Amazon, Dynamixel (ROBOTIS), MuJoCo, Intel, Xbox, Windows, Edge, Minecraft (Microsoft), Chrome,
Android (Google), Safari, iPhone, iPad, macOS (Apple), Firefox (Mozilla), Scratch (MIT), Python
(Python Software Foundation), Claude (Anthropic)) belong to their owners and are used only
descriptively.

**How it was made.** Most of the software and the text was written by an AI system (Claude by
Anthropic) under the author's direction. Idea, architecture, choice of components, review and
responsibility lie with the author; see [About the Author](E-about-the-author.md). This English
edition is a translation of the German original *Kern – Einfach erklärt*; the translation was made
with the same AI system and lies in the author's responsibility as well.

**Liability.** All information, programs and instructions were prepared and checked with care,
without guarantee of correctness or completeness. Anyone who builds, calibrates or operates robots
following this book does so at their own risk; author and rights holder are liable only for intent
and gross negligence, for injury to life, body or health, and under mandatory statutory
provisions, in particular product liability law (Produkthaftungsgesetz).

## Contents

| | |
|---|---|
| **Part I – The Engine** | [1 Introduction](01-introduction.md) · [2 Building and Testing](02-building-and-testing.md) · [3 Foundations](03-foundations.md) · [4 Graphics](04-graphics.md) · [5 Scene and Geometry](05-scene-and-geometry.md) · [6 Physics](06-physics.md) · [7 Audio](07-audio.md) · [8 Game Runtime](08-game-runtime.md) · [9 Logic](09-logic.md) · [10 Networking](10-networking.md) |
| **Part II – Using Kern** | [11 Kern Studio](11-studio.md) · [12 Your Own Games](12-your-own-games.md) |
| **Part III – Robots and Learning** | [13 The Micro Duck](13-micro-duck.md) · [14 The Lehrer](14-lehrer.md) · [15 Training Your Own Models](15-your-own-models.md) · [16 Calibration](16-calibration.md) · [17 Reference](17-reference.md) |
| **Appendices – Background** | [A Neural Networks and Reinforcement Learning](A-neural-networks.md) · [B Real-Time Rendering](B-rendering.md) · [C Physics Simulation](C-physics.md) · [D Licenses](D-licences.md) · [About the Author](E-about-the-author.md) |
