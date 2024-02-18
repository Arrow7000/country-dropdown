# Ahrefs task

You can view this dropdown in action at [ahrefs-country-select.netlify.app](https://ahrefs-country-select.netlify.app/).

The repo for this project is at [github.com/Arrow7000/country-dropdown](https://github.com/Arrow7000/country-dropdown).

## Run

To initialise this project, run the following commands

```sh
opam switch create . --yes
eval $(opam env)
npm install
npm run dev
```

The development server should run on [localhost:5173](http://localhost:5173) and should open a browser window automatically.

## Notes

- The `CountrySelect` component is in the [`src/CountrySelect.re`](src/CountrySelect.re) module. The [`src/App.re`](src/App.re) module shows an example usage of it.
- This project was scaffolded with [create-melange-app](https://github.com/dmmulroy/create-melange-app).
