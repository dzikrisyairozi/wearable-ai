from setuptools import setup, find_packages

setup(
    name="ai-chatbot",
    version="0.1.0",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    install_requires=[
        "transformers==4.37.2",
        "torch==2.2.0",
        "accelerate==0.27.2",
        "discord-webhook==1.3.0",
        "python-dotenv==1.0.1",
        "rich==13.7.0",
    ],
    python_requires=">=3.8",
    author="Your Name",
    author_email="dzikrisyairozi@gmail.com",
    description="A modular CLI chatbot with Discord webhook integration",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/dzikrisyairozi/wearable-ai",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    entry_points={
        "console_scripts": [
            "ai-chat=chatbot.core.cli_interface:main",
        ],
    },
) 