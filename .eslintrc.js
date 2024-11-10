module.exports = {
  root: true,
  parser: "@typescript-eslint/parser",
  parserOptions: {
    project: "./tsconfig.json",
  },
  extends: [
    "airbnb-base",
    "airbnb-typescript/base",
    "eslint:recommended",
    "prettier",
  ],
  plugins: [
    "@typescript-eslint",
    "import",
    "prettier",
  ],
  globals: {
    __DEV__: "readable",
  },
  overrides: [
    {
      files: ["*.ts", "*.tsx"],
      rules: {
        "prettier/prettier": ["error"],
        "no-plusplus": [2, { allowForLoopAfterthoughts: true }],
        "no-continue": "off",
        "class-methods-use-this": "off",
        "@typescript-eslint/no-shadow": ["error"],
        "import/prefer-default-export": "off",
        "no-restricted-exports": "off",
      },
    },
  ],
  settings: {
    react: {
      version: "detect",
    },
    "import/parsers": {
      "@typescript-eslint/parser": [".ts", ".tsx"],
    },
  },
};
