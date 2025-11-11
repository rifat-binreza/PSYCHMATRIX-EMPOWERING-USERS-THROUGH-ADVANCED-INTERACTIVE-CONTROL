import { useColorScheme as _useColorScheme } from 'react-native-appearance';

export const useColorScheme = () => {
  const scheme = _useColorScheme();
  return scheme === 'dark' ? 'dark' : 'light';
};

export default useColorScheme;
