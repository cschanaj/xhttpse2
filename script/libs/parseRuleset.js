"use strict";

/**
 *
 * @param {string} xmlContent
 * @param {RegExp} matchRegex
 * @param {Function} postProcessingCallback
 */
function parseXmlByRegExp(xmlContent, matchRegex, postProcessingCallback) {
  let matches = null;
  while ((matches = matchRegex.exec(xmlContent)) !== null) {
    // This is necessary to avoid infinite loops with zero-width matches
    if (matches.index === matchRegex.lastIndex) {
      matchRegex.lastIndex++;
    }
    postProcessingCallback(matches);
  }
}

/**
 * Perform the heavy lifting of converting a ruleset into a JavaScript object
 * @param {string} xmlContent
 */
function parseRuleset(xmlContent) {
  const ruleset = {
    metadata: {
      name: null,
      references: [],
    },
    target: {
      hosts: [],
    },
    securecookie: false,
  };

  const namedRegExp = {
    metadata: {
      name: [/^\s*<ruleset\s*name="([^"]*)"[^>]*>$/gm],
    },
    target: {
      hosts: [/^\s*-\s*([^>\n]*)$/gm, /^\s*<target\s*host="([^"]*)"\s*\/>$/gm],
    },
    securecookie: [/<securecookie\s*host="[^"]*"\s*name="[^"]*"\s*\/>/gm],
  };

  namedRegExp.metadata.name.forEach((regex) => {
    parseXmlByRegExp(xmlContent, regex, (matches) => {
      ruleset.metadata.name = matches[1];
    });
  });

  namedRegExp.target.hosts.forEach((regex) => {
    parseXmlByRegExp(xmlContent, regex, (matches) => {
      ruleset.target.hosts.push(matches[1]);
    });
  });

  namedRegExp.securecookie.forEach((regex) => {
    parseXmlByRegExp(xmlContent, regex, (matches) => {
      ruleset.securecookie = true;
    });
  });

  return ruleset;
}

module.exports = parseRuleset;
